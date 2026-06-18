#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness(void) {
    bool use_static = __CPROVER_nondet_bool();
    struct aws_string *str = NULL;

    if (use_static) {
        AWS_STATIC_STRING_FROM_LITERAL(static_str, "static_test");
        str = (struct aws_string *)static_str;
    } else {
        const char *c_str = "dynamic_test";
        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);
        str = aws_string_new_from_c_str(allocator, c_str);
    }

    /* Ensure the precondition for aws_string_destroy_secure */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    aws_string_destroy_secure(str);
}
