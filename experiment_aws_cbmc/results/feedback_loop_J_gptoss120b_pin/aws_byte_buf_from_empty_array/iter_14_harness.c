#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_CAPACITY 1024

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_CAPACITY));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t capacity;
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    struct aws_byte_buf old_buf = buf;

    int result = aws_byte_buf_from_empty_array(&buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
            assert(buf.len == 0);
            assert(buf.capacity == 0);
        } else {
            assert(buf.buffer != NULL);
            assert(buf.len == 0);
            assert(buf.capacity == capacity);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }
}
