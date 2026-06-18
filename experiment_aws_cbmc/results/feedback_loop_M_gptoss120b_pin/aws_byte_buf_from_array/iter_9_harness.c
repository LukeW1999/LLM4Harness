#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf dest;
    const uint8_t *bytes;
    size_t len = nondet_size_t();

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    if (len > 0) {
        uint8_t *buf = malloc(len);
        __CPROVER_assume(buf != NULL);
        bytes = buf;
    } else {
        bytes = NULL;
    }

    const uint8_t *old_bytes = bytes;

    int result = aws_byte_buf_from_array(&dest, bytes, len);

    __CPROVER_assert(result == AWS_OP_SUCCESS, "aws_byte_buf_from_array returns success");
    __CPROVER_assert(dest.buffer == (uint8_t *)bytes, "dest.buffer points to input bytes");
    __CPROVER_assert(dest.len == len, "dest.len is set correctly");
    __CPROVER_assert(dest.capacity == len, "dest.capacity is set correctly");
    __CPROVER_assert(dest.allocator == NULL, "dest.allocator is NULL");

    if (len > 0) {
        __CPROVER_assert(dest.buffer != NULL, "dest.buffer is non-NULL when len > 0");
    } else {
        __CPROVER_assert(dest.buffer == NULL, "dest.buffer is NULL when len == 0");
    }

    __CPROVER_assert(bytes == old_bytes, "bytes pointer unchanged");
    __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest is a valid aws_byte_buf");
}
