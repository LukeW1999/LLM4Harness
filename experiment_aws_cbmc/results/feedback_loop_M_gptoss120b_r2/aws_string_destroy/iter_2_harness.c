#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
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

        /* 3. Allocate storage for the struct plus the flexible array on the stack */
        uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)storage;

        /* 4. Initialise fields */
        str->allocator = NULL;               /* no allocator, so destroy does not free */
        *((size_t *)&str->len) = len;        /* cast away const to set length */

        /* 5. Fill the byte payload with non‑deterministic data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* 6. Assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* 7. Save old state for the unchanged‑field checks */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old = *str;                                 /* copy the header */
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* 8. Call the function under test */
    aws_string_destroy(str);

    /* 9. Post‑condition checks */
    if (str == NULL) {
        /* nothing to check – the function is a no‑op */
    } else {
        /* allocator is NULL → no free, all fields must be unchanged */
        assert(str->allocator == old.allocator);
        assert(str->len == old.len);
        assert_bytes_match(str->bytes, old.bytes, str->len);
        /* validity must still hold */
        assert(aws_string_is_valid(str));
    }
}
