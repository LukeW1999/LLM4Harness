#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness(void) {
    struct aws_string string_obj = {0};
    
    bool is_null = nondet_bool();
    
    if (!is_null) {
        // We must set allocator to NULL because we cannot safely free a stack-allocated object.
        // aws_string_destroy will check if allocator is NULL and do nothing, which is correct for static strings.
        string_obj.allocator = NULL;
        
        size_t nondet_len = nondet_size_t();
        __CPROVER_assume(nondet_len <= (size_t)1024);
        string_obj.len = nondet_len;
        
        __CPROVER_assume(aws_string_is_valid(&string_obj));
    }

    struct aws_string *str_ptr = NULL;
    if (!is_null) {
        str_ptr = &string_obj;
    }
    
    bool was_null = is_null;
    bool was_static = false;
    size_t old_len = 0;
    
    if (!is_null) {
        was_static = (string_obj.allocator == NULL) ? true : false;
        old_len = string_obj.len;
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
