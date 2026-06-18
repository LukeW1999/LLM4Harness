#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    /* Declare and bound inputs */
    struct aws_allocator *allocator = aws_default_allocator();
    
    /* Non-deterministic C-string: we need a bounded buffer */
    size_t c_str_len;
    char *c_str = NULL;
    
    /* Create a bounded C-string input */
    c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    
    c_str = malloc(c_str_len + 1); /* +1 for null terminator */
    __CPROVER_assume(c_str != NULL);
    
    /* Fill with non-deterministic bytes, ensure null-terminated */
    for (size_t i = 0; i < c_str_len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';
    
    /* We need to ensure the string content is valid (any byte values are fine) */
    /* Just ensure memory is readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    
    /* Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);
    
    /* Postconditions */
    if (result != NULL) {
        /* Success path */
        assert(allocator == result->allocator);
        assert(c_str_len == result->len);
        assert(aws_string_is_valid(result));
        
        /* Check bytes match */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, c_str_len);
        
        /* Ensure null-terminated */
        assert(result->bytes[result->len] == 0);
    } else {
        /* Failure path: result is NULL */
        /* No structure to assert unchanged, but we can still assert nothing else changed */
    }
    
    /* Cleanup */
    free(c_str);
    
    /* If result was allocated, we should clean it up, but in harness we just test the creation */
    if (result != NULL) {
        /* Note: In a real harness we might want to free, but it's optional for CBMC */
        /* aws_string_destroy(result);  // would need the destructure function, but it's harmless */
    }
}
