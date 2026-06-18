#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_BUFFER_SIZE);
    
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    
    for (size_t i = 0; i < len; i++) {
        c_str[i] = (char)nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[len] = '\0';
    
    __CPROVER_assume(aws_c_string_is_valid(c_str));
    
    struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);
    
    if (str != NULL) {
        assert(aws_string_is_valid(str));
        assert(str->allocator == allocator);
        assert(str->len == len);
        for (size_t i = 0; i < len; i++) {
            assert(str->bytes[i] == (uint8_t)c_str[i]);
        }
        assert(str->bytes[len] == '\0');
    }
    
    assert(c_str[len] == '\0');
}
