#include <aws/common/byte_buf.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness() {
    char c_str_arr[10];
    c_str_arr[9] = '\0';
    const char *c_str = nondet_bool() ? NULL : c_str_arr;

    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    size_t expected_len = 0;
    if (c_str != NULL) {
        while (c_str[expected_len] != '\0') {
            expected_len++;
        }
    }

    if (c_str == NULL || expected_len == 0) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        assert(result.buffer == (uint8_t *)c_str);
    }
    
    assert(result.allocator == NULL);
    assert(aws_byte_buf_is_valid(&result));
}
