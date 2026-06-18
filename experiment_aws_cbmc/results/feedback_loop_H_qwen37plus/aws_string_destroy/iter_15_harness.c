#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *str = can_fail_malloc(sizeof(struct aws_string) + 1024);
    bool is_static = nondet_bool();
    
    if (str != NULL) {
        if (is_static) {
            str->allocator = NULL;
        } else {
            str->allocator = allocator;
        }
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= 1024);
        str->bytes[str->len] = '\0';
        __CPROVER_assume(aws_string_is_valid(str));
    }

    bool was_null = (str == NULL);
    bool was_static_alloc = (str != NULL && str->allocator == NULL);
    size_t old_len = (str != NULL) ? str->len : 0;

    aws_string_destroy(str);

    if (was_null) {
        assert(str == NULL);
    } else if (was_static_alloc) {
        assert(str != NULL);
        assert(str->allocator == NULL);
        assert(str->len == old_len);
        assert(aws_string_is_valid(str));
    }
}
