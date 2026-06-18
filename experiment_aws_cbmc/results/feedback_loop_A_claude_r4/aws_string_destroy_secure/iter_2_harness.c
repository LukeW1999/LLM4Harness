#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool use_null;
    
    if (use_null) {
        /* Case 1: NULL input — function should be a no-op */
        aws_string_destroy_secure(NULL);
        assert(true);
    } else {
        /* Case 2: Valid string */
        struct aws_allocator *allocator = aws_default_allocator();
        
        size_t len;
        __CPROVER_assume(len <= 10);
        
        uint8_t bytes[10] = {0};
        
        struct aws_string *str = aws_string_new_from_array(allocator, bytes, len);
        
        if (str != NULL) {
            assert(aws_string_is_valid(str));
            
            aws_string_destroy_secure(str);
            
            assert(true);
        } else {
            aws_string_destroy_secure(NULL);
            assert(true);
        }
    }
}
