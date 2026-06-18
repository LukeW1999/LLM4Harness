#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buf;

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    if (aws_ring_buffer_init(&ring_buf, allocator, allocation_size) != AWS_OP_SUCCESS) {
        return;
    }

    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset < allocation_size);
    __CPROVER_assume(tail_offset < allocation_size);

    aws_atomic_init_ptr(&ring_buf.head, ring_buf.allocation + head_offset);
    aws_atomic_init_ptr(&ring_buf.tail, ring_buf.allocation + tail_offset);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest = {0};

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);
    assert(new_tail == old_tail);

    if (result == AWS_OP_SUCCESS) {
        assert(new_head != old_head);
        assert(dest.allocator == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        assert(new_head == old_head);
    }

    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
}
