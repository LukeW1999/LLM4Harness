#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

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
    assert(result == AWS_OP_SUCCESS);
    assert(buf.buffer == bytes);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
