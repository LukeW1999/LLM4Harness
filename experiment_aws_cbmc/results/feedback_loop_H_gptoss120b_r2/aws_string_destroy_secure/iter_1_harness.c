#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate memory for the struct plus the flexible array */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose an allocator (NULL or default) */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = len;

        /* initialise the bytes with nondeterministic values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    if (str) {
        /* Save old immutable fields */
        const struct aws_allocator *old_allocator = str->allocator;
        const size_t old_len = str->len;
        const bool old_allocator_is_null = (old_allocator == NULL);

        /* Save a copy of the original bytes for later comparison */
        uint8_t *old_bytes = NULL;
        if (old_len > 0) {
            old_bytes = (uint8_t *)malloc(old_len);
            __CPROVER_assume(old_bytes != NULL);
            const uint8_t *cur_bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                old_bytes[i] = cur_bytes[i];
            }
        }

        /* Call the function under test */
        aws_string_destroy_secure(str);

        if (old_allocator_is_null) {
            /* The string was not dynamically allocated, so it should still be valid.
               Its bytes must have been zeroed, and immutable fields unchanged. */
            const uint8_t *cur_bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(cur_bytes[i] == 0);
            }
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            assert(aws_string_is_valid(str));
        } else {
            /* The string was dynamically allocated and has been released.
               The memory may no longer be accessible; we must not dereference `str`. */
            /* No further assertions about the contents of `str` are permitted. */
        }

        if (old_bytes) {
            free(old_bytes);
        }
    } else {
        /* str is NULL – the function should be a no‑op. */
        aws_string_destroy_secure(str);
        /* No state to check; simply ensure no assertion failures. */
    }
}
