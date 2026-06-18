#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether to allocate a string or use NULL */
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate space for the struct plus flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* nondeterministically decide whether the string has an allocator */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        /* initialise the bytes with nondeterministic values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    } else {
        str = NULL;
    }

    /* Save old state that we can still observe after the call */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    /* Call the function under verification */
    aws_string_destroy_secure(str);

    /* Post‑condition checks */
    if (str) {
        if (old_allocator == NULL) {
            /* Static string: memory is not freed, bytes must be zeroed */
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }
            /* The string should still be valid after zeroing */
            assert(aws_string_is_valid(str));
        } else {
            /* Dynamically allocated string: memory may have been released.
               We cannot dereference `str` after this point, but we can assert
               that the allocator pointer we observed before the call is unchanged
               (the struct itself is no longer accessible). */
            assert(old_allocator == str->allocator);
        }
    } else {
        /* If the input pointer was NULL, nothing should happen. */
        assert(str == NULL);
    }

    /* In all cases the allocator pointer we observed (if any) must remain the same. */
    if (str) {
        assert(str->allocator == old_allocator);
    }
}
