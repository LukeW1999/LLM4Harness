#include <aws/common/string.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str;
    
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);
    
    if (is_null) {
        str = NULL;
    } else {
        size_t len;
        __CPROVER_assume(len <= 10);
        
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(alloc_size >= sizeof(struct aws_string) + 1);
        
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        
        struct aws_allocator **allocator_ptr = (struct aws_allocator **)&str->allocator;
        
        bool has_allocator;
        __CPROVER_assume(has_allocator == 0 || has_allocator == 1);
        
        if (has_allocator) {
            *allocator_ptr = aws_default_allocator();
        } else {
            *allocator_ptr = NULL;
        }
        
        size_t *len_ptr = (size_t *)&str->len;
        *len_ptr = len;
        
        uint8_t *bytes_ptr = (uint8_t *)str->bytes;
        bytes_ptr[len] = 0;
    }
    
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    
    bool str_was_null = (str == NULL);
    bool had_null_allocator = (str != NULL && str->allocator == NULL);
    
    aws_string_destroy(str);
    
    assert(true);
    
    if (str_was_null) {
        assert(str == NULL);
    }
    
    if (had_null_allocator) {
        assert(str->allocator == NULL);
        assert(aws_string_is_valid(str));
    }
    
    if (str_was_null) {
        struct aws_allocator *alloc = aws_default_allocator();
        assert(alloc != NULL);
    }
}
