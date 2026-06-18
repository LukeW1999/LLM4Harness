#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    size_t max_len = MAX_BUFFER_SIZE;
    size_t len = nondet_size_t();
    __CPROVER_assume(len < max_len);
    
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    
    for (size_t i = 0; i < len; i++) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[len] = '\0';
    
    struct aws_string *result = aws_string_new_from_c_str(aws_default_allocator(), c_str);
    
    if (result != NULL) {
        assert(aws_string_is_valid(result));
        assert(result->len == len);
        for (size_t i = 0; i < len; i++) {
            assert(result->bytes[i] == (uint8_t)c_str[i]);
        }
        assert(result->bytes[len] == '\0');
        aws_string_destroy(result);
    }
    
    free(c_str);
}
