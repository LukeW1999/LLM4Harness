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
        str = aws_string_new_from_c_str(allocator, c_str);
    }

    struct aws_string *old_str = str;
    size_t old_len = (old_str != NULL) ? old_str->len : 0;
    struct aws_allocator *old_allocator = (old_str != NULL) ? old_str->allocator : NULL;

    size_t i = __CPROVER_nondet_size_t();
    __CPROVER_assume(i < old_len);

    aws_string_destroy_secure(str);

    if (old_str != NULL && old_allocator == NULL) {
        const uint8_t *post_bytes = aws_string_bytes(old_str);
        for (size_t j = 0; j < old_len; ++j) {
            assert(post_bytes[j] == 0);
        }
        assert(old_str->len == old_len);
    } else if (old_str != NULL && old_allocator != NULL) {
        assert(old_allocator != NULL);
    } else {
        assert(str == NULL);
    }
}
