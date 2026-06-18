#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    buf.allocator = allocator;

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.len <= buf.capacity);

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_reset(&buf);

    assert(buf.len == 0);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
}
