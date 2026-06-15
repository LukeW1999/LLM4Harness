#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 10

void aws_byte_buf_from_c_str_harness() {
    char c_str[MAX_BUFFER_SIZE];
    size_t str_len;
    
    /* Allow empty strings (null_pos = 0) */
    __CPROVER_assume(str_len < MAX_BUFFER_SIZE);
    
    /* Initialize with non-null characters up to str_len */
    for (size_t i = 0; i < str_len; i++) {
        c_str[i] = nondet_uint8_t();
        __CPROVER_assume(c_str[i] != 0);
    }
    c_str[str_len] = '\0';
    
    /* Ensure there are no embedded nulls */
    for (size_t i = str_len + 1; i < MAX_BUFFER_SIZE; i++) {
        c_str[i] = nondet_uint8_t();
    }

    /* Call the function */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Postconditions */
    assert(buf.len == str_len);
    assert(buf.capacity == str_len);
    assert(buf.buffer == (uint8_t *)c_str);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
