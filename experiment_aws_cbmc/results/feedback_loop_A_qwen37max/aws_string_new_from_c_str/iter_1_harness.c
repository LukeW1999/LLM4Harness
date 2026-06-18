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
    c_str[len] = '\0';
    
    size_t actual_len = 0;
    while (c_str[actual_len] != '\0') {
        actual_len++;
    }
    
    struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);
    
    if (str != NULL) {
        assert(aws_string_is_valid(str));
        assert(str->len == actual_len);
        assert(str->allocator == allocator);
        assert_bytes_match((const uint8_t *)str->bytes, (const uint8_t *)c_str, actual_len);
        assert(str->bytes[actual_len] == '\0');
    }
}
