#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf = buf;

    struct aws_allocator *alloc = aws_default_allocator();

    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.allocator == alloc);
        assert(buf.capacity == capacity);
        assert(buf.len == 0);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    /* Ensure the buffer structure was not unintentionally modified beyond the
       expected fields on failure. */
    if (result != AWS_OP_SUCCESS) {
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
