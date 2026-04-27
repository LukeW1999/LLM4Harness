#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness() {
    struct aws_byte_buf buf;
    buf.buffer = (uint8_t *)NULL;
    buf.len = 0;
    buf.capacity = 0;
    buf.allocator = NULL;

    size_t capacity = nondet_size_t();

    int result = aws_byte_buf_from_empty_array(&buf, capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.buffer != NULL);
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == NULL);
    } else {
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
