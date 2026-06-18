#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    bool is_null = nondet_bool();
    struct aws_string *str = NULL;
    bool has_allocator = false;
    
    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[MAX_BUFFER_SIZE];
        } *s = malloc(sizeof(*s));
        __CPROVER_assume(s != NULL);
        
        has_allocator = nondet_bool();
        if (has_allocator) {
            s->allocator = aws_default_allocator();
        } else {
            s->allocator = NULL;
        }
        s->len = len;
        
        str = (struct aws_string *)s;
    }
    
    struct store_byte_from_buffer old_byte;
    if (str != NULL && str->len > 0) {
        save_byte_from_array(aws_string_bytes(str), str->len, &old_byte);
    }
    
    aws_string_destroy_secure(str);
    
    if (str != NULL && !has_allocator) {
        if (str->len > 0) {
            assert(aws_string_bytes(str)[old_byte.index] == 0);
        }
        assert(aws_string_is_valid(str));
    }
}
