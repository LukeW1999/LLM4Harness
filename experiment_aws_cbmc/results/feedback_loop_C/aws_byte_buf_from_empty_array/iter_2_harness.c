#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_byte_buf_from_empty_array_harness() {
    struct aws_byte_buf buf;
    buf.buffer = (uint8_t *)nondet_size_t();
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)nondet_size_t();

    struct aws_byte_buf old_buf = buf;
    size_t capacity = nondet_size_t();

    int result = aws_byte_buf_from_empty_array(&buf, capacity);

    if (result == AWS_OP_SUCCESS) {
        if (capacity > 0) {
            assert(buf.buffer != NULL);
        } else {
            assert(buf.buffer == NULL);
        }
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == NULL);
    } else {
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
