#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str;
    bool have_str = nondet_bool();

    if (have_str) {
        /* Allocate a buffer on the stack large enough for the string object */
        uint8_t buffer[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)buffer;
        __CPROVER_assume(str != NULL);

        /* Nondeterministically decide whether the string carries an allocator */
        bool have_allocator = nondet_bool();
        if (have_allocator) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* Nondeterministic length, bounded */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_STRING_LEN);

        /* Assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* Save a shallow copy of the original fields */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    uint8_t old_first_byte = 0;

    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        if (old_len > 0) {
            old_first_byte = str->bytes[0];
        }
    }

    /* Call the function under test */
    aws_string_destroy(str);

    /* Post‑conditions */
    if (str != NULL) {
        if (old_allocator != NULL) {
            /* Memory may have been released; we must not dereference `str`. */
        } else {
            /* No allocator: the function should not modify the object. */
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            if (old_len > 0) {
                assert(str->bytes[0] == old_first_byte);
            }
        }
    }

    /* Validity invariant for the cases where the object remains allocated */
    if (str != NULL && old_allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
