#include <aws/common/byte_buf.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 8

void aws_byte_buf_from_c_str_harness(void) {
    /* Non-deterministically choose a c_str */
    char *c_str;
    
    /* Use nondet to decide if NULL or valid string */
    int is_null;
    
    if (is_null) {
        c_str = NULL;
    } else {
        /* Allocate a bounded string for CBMC */
        static char buffer[MAX_BUFFER_SIZE + 1];
        /* Assume it's a valid zero-terminated string */
        __CPROVER_assume(__CPROVER_is_zero_string(buffer));
        __CPROVER_assume(__CPROVER_zero_string_length(buffer) <= MAX_BUFFER_SIZE);
        c_str = buffer;
    }
    
    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);
    
    /* === Assertions for NULL case === */
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
        assert(result.allocator == NULL);
    }
    
    /* === Assertions for non-NULL case === */
    if (c_str != NULL) {
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        assert(result.buffer == (uint8_t *)c_str);
        assert(result.allocator == NULL);
    }
    
    /* === Validity invariant === */
    assert(aws_byte_buf_is_valid(&result));
}
