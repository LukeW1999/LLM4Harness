#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    /* 1. Non‑deterministically decide whether we have a string */
    struct aws_string *str;
    bool have_str = nondet_bool();
    __CPROVER_assume(have_str || !have_str); /* keep CBMC happy */

    if (have_str) {
        /* 2. Choose a non‑deterministic length bounded by MAX_BUFFER_SIZE */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* 3. Allocate memory for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* 4. Non‑deterministically decide whether the string has an allocator */
        bool has_allocator = nondet_bool();
        if (has_allocator) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* 5. Initialise the const length field (cast away const) */
        *((size_t *)&str->len) = len;

        /* 6. Fill the byte payload with non‑deterministic data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* 7. Assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* 8. Save old state for the unchanged‑field checks */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old = *str;                                 /* copy the header */
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* 9. Call the function under test */
    aws_string_destroy(str);

    /* 10. Post‑condition checks */
    if (str == NULL) {
        /* nothing to check – the function is a no‑op */
    } else {
        if (str->allocator == NULL) {
            /* allocator absent → no free, all fields must be unchanged */
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            assert_bytes_match(str->bytes, old.bytes, str->len);
            /* validity must still hold */
            assert(aws_string_is_valid(str));
        } else {
            /* allocator present → memory was released.
               The pointer may now be dangling; we must not dereference it.
               The only guarantee is that the function does not corrupt
               unrelated memory, which is ensured by the allocator model. */
            /* No field dereference here. */
        }
    }
}
