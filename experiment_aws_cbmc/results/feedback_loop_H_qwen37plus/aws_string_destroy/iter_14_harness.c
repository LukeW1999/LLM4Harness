#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str_ptr = NULL;
    bool is_null = nondet_bool();
    
    if (!is_null) {
        struct aws_string *string_obj = can_fail_malloc(sizeof(struct aws_string) + 1024);
        if (string_obj != NULL) {
            string_obj->allocator = aws_default_allocator();
            size_t nondet_len = nondet_size_t();
            __CPROVER_assume(nondet_len <= (size_t)1024);
            string_obj->len = nondet_len;
            string_obj->bytes[nondet_len] = '\0';
            __CPROVER_assume(aws_string_is_valid(string_obj));
            str_ptr = string_obj;
        }
    }

    bool was_null = is_null;
    bool was_static = false;
    size_t old_len = 0;
    
    if (!is_null && str_ptr != NULL) {
        was_static = (str_ptr->allocator == NULL) ? true : false;
        old_len = str_ptr->len;
    }

    aws_string_destroy(str_ptr);

    if (was_null) {
        assert(str_ptr == NULL);
    } else if (was_static) {
        assert(str_ptr != NULL);
        assert(str_ptr->allocator == NULL);
        assert(str_ptr->len == old_len);
        assert(aws_string_is_valid(str_ptr));
    }
}
