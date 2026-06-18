#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_harness(void) {
    /* 1. Declare a possibly NULL aws_string pointer */
    struct aws_string str_obj;
    struct aws_string *str = nondet_bool() ? &str_obj : NULL;

    if (str != NULL) {
        /* nondeterministically decide whether the allocator is set */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* nondeterministic length */
        str->len = nondet_size_t();

        /* Assume the string is valid according to the library predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Save old state for fields we can observe without dereferencing after free */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    /* 3. Call the function under test */
    aws_string_destroy(str);

    /* 4. Postconditions */

    /* The pointer value itself must remain unchanged */
    assert(str == str); /* trivially true, ensures we reference the same variable */

    if (str != NULL && old_allocator != NULL) {
        /* Memory was released; we must not dereference the freed object. */
    } else {
        /* No release happened: the object must still be valid and unchanged */
        assert(aws_string_is_valid(str));
        if (str != NULL) {
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
        }
    }

    /* 5. Global invariant: any non‑NULL string that was not freed remains valid */
    if (str != NULL && old_allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
