#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness(void) {
    /* 1. Declare pointer and nondeterministically decide if it is NULL */
    struct aws_string *str;
    bool make_null = nondet_bool();
    if (make_null) {
        str = NULL;
    } else {
        /* 2. Non‑deterministic length bounded by MAX_BUFFER_SIZE */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* 3. Allocate enough memory for the struct plus the flexible array */
        size_t total_size = sizeof(struct aws_string) + len;
        str = (struct aws_string *)malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* 4. Set fields */
        bool allocator_null = nondet_bool();
        if (allocator_null) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }
        str->len = len;

        /* 5. Initialise the byte contents nondeterministically */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* 6. Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 7. Save old state for immutability checks */
    struct aws_string old;
    if (str) {
        old.allocator = str->allocator;
        old.len = str->len;
    }

    /* 8. Call function under test */
    aws_string_destroy_secure(str);

    /* 9. Post‑condition checks */
    if (str == NULL) {
        /* No effect when input is NULL */
        assert(str == NULL);
    } else {
        if (old.allocator == NULL) {
            /* When no allocator, memory is not released.
               The bytes must be zeroed and fields unchanged. */
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old.len; ++i) {
                assert(bytes[i] == 0);
            }
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            /* Validity must still hold */
            assert(aws_string_is_valid(str));
        } else {
            /* When an allocator is present, the string may have been freed.
               We must not dereference `str` after the call. No further asserts. */
        }
    }
}
