#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    ensure_byte_buf_has_allocated_buffer_member(&ring_buf.allocation);
    __CPROVER_assume(ring_buf.allocation != NULL);
    __CPROVER_assume(ring_buf.allocation_end != NULL);
    ring_buf.allocator = aws_default_allocator();

    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == requested_size);
        assert(dest.capacity >= requested_size);
        assert(dest.buffer != NULL);
        assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    } else {
        assert(ring_buf.head == old_ring_buf.head);
        assert(ring_buf.tail == old_ring_buf.tail);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.buffer == old_dest.buffer);
    }

    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&dest));
}
