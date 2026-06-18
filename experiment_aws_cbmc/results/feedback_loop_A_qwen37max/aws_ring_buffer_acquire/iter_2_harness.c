#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;
    
    size_t size;
    __CPROVER_assume(size > 0 && size <= 100);
    
    ring_buf.allocation = malloc(size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + size;

    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= size);
    __CPROVER_assume(tail_offset <= size);

    aws_atomic_store_ptr(&ring_buf.head, ring_buf.allocation + head_offset);
    aws_atomic_store_ptr(&ring_buf.tail, ring_buf.allocation + tail_offset);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;

    size_t requested_size;
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= 100);

    struct aws_byte_buf dest = {0};

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.buffer != NULL);
    } else {
        assert((uint8_t *)aws_atomic_load_ptr(&ring_buf.head) == old_head);
        assert((uint8_t *)aws_atomic_load_ptr(&ring_buf.tail) == old_tail);
    }

    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
