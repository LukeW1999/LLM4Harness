#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness() {
    size_t capacity;
    uint8_t array[MAX_BUFFER_SIZE];

    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
    if (buf.buffer) {
        assert_bytes_match(buf.buffer, array, capacity);
    }
}
