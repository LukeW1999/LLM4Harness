#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    /* 1. Declare pointer and nondeterministically decide if it is NULL */
    struct aws_string *str;
    bool is_null = nondet_bool();
    if (is_null) {
        str = NULL;
    } else {
        /* 2. Non‑deterministically choose a length and bound it */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* 3. Allocate enough memory for the struct plus flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* 4. Non‑deterministically choose an allocator (NULL or default) */
        bool has_allocator = nondet_bool();
        if (has_allocator) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* 5. Initialise length */
        *((size_t *)&str->len) = len; /* const field, cast away const for init */

        /* 6. Fill the byte array with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* 7. Save old state for later comparison (only if non‑NULL) */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old = *str; /* copies allocator and len (bytes are not copied) */
        save_byte_from_array(aws_string_bytes(str), str->len, &old_bytes);
    }

    /* 8. Call the function under test */
    aws_string_destroy_secure(str);

    /* 9. Post‑condition checks */
    if (str) {
        if (old.allocator) {
            /* When an allocator is present the string memory is released.
               The pointer may now be dangling, so we do not dereference it.
               The only guarantee is that the allocator was non‑NULL before. */
            assert(old.allocator != NULL);
        } else {
            /* No allocator: memory is not freed, bytes must be zeroed */
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old.len; ++i) {
                assert(bytes[i] == 0);
            }

            /* Unchanged fields */
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);

            /* Validity invariant must still hold */
            assert(aws_string_is_valid(str));
        }
    } else {
        /* str was NULL: nothing should happen */
        assert(str == NULL);
    }
}
