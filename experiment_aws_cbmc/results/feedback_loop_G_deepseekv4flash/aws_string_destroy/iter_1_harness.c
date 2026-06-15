#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness() {
    /* Non-deterministic allocation of the string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_STRING_LEN); /* bound to avoid unbounded memory */

    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    if (str) {
        /* Non-deterministic allocator: either NULL (static) or valid (dynamic) */
        str->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        str->len = len;
    }

    /* Assume the string is valid if non-NULL */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    /* Save old state only for strings that will NOT be freed (allocator == NULL) */
    struct aws_string old;
    bool is_static = (str != NULL && str->allocator == NULL);
    if (is_static) {
        old = *str;
    }

    /* Call the function */
    aws_string_destroy(str);

    /* Postcondition: static strings (allocator == NULL) are unchanged */
    if (is_static) {
        assert(str->allocator == old.allocator);
        assert(str->len == old.len);
        assert(aws_string_is_valid(str));
    }

    /* For dynamic strings, the memory is freed so we cannot access it.
     * The function must not crash, which is verified by CBMC automatically.
     * This trivial assert ensures the harness has at least one assertion. */
    assert(1);
}
