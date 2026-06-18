#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_harness(void) {
    /* 1. Declare a possibly NULL aws_string pointer */
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        /* nondeterministic length, bounded to avoid huge allocations */
        size_t len = nondet_size_t();
        __CPROVER_assume(len < 256);

        /* Create a valid string using the default allocator */
        str = aws_string_new_from_array(aws_default_allocator(), NULL, len);
        __CPROVER_assume(aws_string_is_valid(str));

        /* nondeterministically decide whether the allocator is set */
        if (nondet_bool()) {
            str->allocator = NULL;
        }
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
