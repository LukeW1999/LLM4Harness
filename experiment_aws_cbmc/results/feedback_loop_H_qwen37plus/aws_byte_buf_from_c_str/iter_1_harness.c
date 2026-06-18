#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness(void) {
    const char *c_str;
    if (c_str != NULL) {
        __CPROVER_assume(__CPROVER_is_zero_string(c_str));
        __CPROVER_assume(strlen(c_str) <= MAX_BUFFER_SIZE);
    }

    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        assert(result.buffer == (uint8_t *)c_str);
    }
    assert(result.allocator == NULL);
    assert(aws_byte_buf_is_valid(&result));
}
