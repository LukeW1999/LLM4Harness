#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_string_new_from_c_str_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    
    size_t len;
    __CPROVER_assume(len < MAX_BUFFER_SIZE);
    
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[len] = '\0';
    
    size_t actual_len = strlen(c_str);
    
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);
    
    if (result != NULL) {
        assert(result->allocator == allocator);
        assert(result->len == actual_len);
        assert_bytes_match((const uint8_t *)c_str, aws_string_bytes(result), actual_len);
        assert(aws_string_is_valid(result));
        aws_string_destroy(result);
    }
    
    free(c_str);
}
