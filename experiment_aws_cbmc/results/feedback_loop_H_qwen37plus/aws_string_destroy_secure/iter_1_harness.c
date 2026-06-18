#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    struct aws_string *str = NULL;
    bool is_null = nondet_bool();
    bool allocator_is_null = false;
    
    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        
        str->len = len;
        allocator_is_null = nondet_bool();
        if (allocator_is_null) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }
    }
    
    aws_string_destroy_secure(str);
    
    if (str != NULL && allocator_is_null) {
        assert(aws_string_is_valid(str));
        const uint8_t *bytes = aws_string_bytes(str);
        for (size_t i = 0; i < str->len; i++) {
            assert(bytes[i] == 0);
        }
    }
}
