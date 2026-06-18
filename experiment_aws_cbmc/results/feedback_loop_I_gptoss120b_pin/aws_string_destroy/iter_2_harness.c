#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str;
    bool have_str = nondet_bool();

    if (have_str) {
        /* Allocate memory for the string (including flexible array space) */
        struct aws_allocator *alloc = aws_default_allocator();
        str = (struct aws_string *)alloc->mem_acquire(
            alloc, sizeof(struct aws_string) + MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);

        /* Nondeterministically decide whether the string carries an allocator */
        bool have_allocator = nondet_bool();
        if (have_allocator) {
            str->allocator = alloc;
        } else {
            str->allocator = NULL;
        }

        /* Nondeterministic length, bounded */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_STRING_LEN);

        /* The bytes are not accessed by aws_string_destroy, no need to initialise */

        /* Assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* Save a shallow copy of the original fields */
    struct aws_string old;
    if (str != NULL) {
        old = *str;
        if (old.len > 0) {
            old.bytes[0] = str->bytes[0];
        }
    }

    /* Call the function under test */
    aws_string_destroy(str);

    /* Post‑conditions */
    if (str != NULL) {
        if (old.allocator != NULL) {
            /* Memory may have been released; we must not dereference `str`. */
        } else {
            /* No allocator: the function should not modify the object. */
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            if (old.len > 0) {
                assert(str->bytes[0] == old.bytes[0]);
            }
            /* Clean up the memory that was not freed by the function. */
            struct aws_allocator *alloc = aws_default_allocator();
            alloc->mem_release(alloc, str);
        }
    }

    /* Validity invariant for the cases where the object remains allocated */
    if (str != NULL && old.allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
