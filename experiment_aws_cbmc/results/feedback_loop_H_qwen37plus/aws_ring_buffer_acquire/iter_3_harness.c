#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity;
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= 1024);

    struct aws_ring_buffer ring_buf;
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, capacity);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    size_t requested_size;
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= 2048);

    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer < ring_buf.allocation_end);
        assert((size_t)(dest.buffer + requested_size) <= (size_t)ring_buf.allocation_end);
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.buffer == old_dest.buffer);
    }

    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    assert(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);
}
