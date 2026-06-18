#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf buf;
    uint8_t *bytes;
    size_t len;

    __CPROVER_assume(len <= 1024);

    bytes = (uint8_t *)malloc(len);
    if (len > 0) {
        __CPROVER_assume(bytes != NULL);
    }

    int result = aws_byte_buf_from_array(&buf, bytes, len);
    __CPROVER_assert(result == AWS_OP_SUCCESS, "aws_byte_buf_from_array should succeed");
    __CPROVER_assert(buf.buffer == bytes, "buf.buffer should equal input bytes");
    __CPROVER_assert(buf.len == len, "buf.len should equal input length");
    __CPROVER_assert(buf.capacity == len, "buf.capacity should equal input length");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator should be NULL");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf should be valid");
}
