#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    struct {
        struct aws_string str;
        uint8_t buffer[1024];
    } string_obj = {0};

    bool is_null = nondet_bool();
    bool is_static = nondet_bool();
    
    if (!is_null) {
        if (is_static) {
            string_obj.str.allocator = NULL;
        } else {
            string_obj.str.allocator = aws_default_allocator();
        }
        
        string_obj.str.len = nondet_size_t();
        __CPROVER_assume(string_obj.str.len <= 1024);
        
        __CPROVER_assume(aws_string_is_valid(&string_obj.str));
    }

    struct aws_string *str_ptr = NULL;
    if (!is_null) {
        str_ptr = &string_obj.str;
    }
    
    bool was_null = is_null;
    bool was_static = !is_null && string_obj.str.allocator == NULL;
    
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (!is_null) {
        old_allocator = string_obj.str.allocator;
        old_len = string_obj.str.len;
    }

    aws_string_destroy(str_ptr);

    if (was_null) {
        assert(str_ptr == NULL);
    } else if (was_static) {
        assert(str_ptr != NULL);
        assert(str_ptr->allocator == old_allocator);
        assert(str_ptr->len == old_len);
        assert(aws_string_is_valid(str_ptr));
    } else {
        /* Dynamically allocated string is freed.
         * We cannot assert anything about `str` as it is deallocated. */
    }
}
