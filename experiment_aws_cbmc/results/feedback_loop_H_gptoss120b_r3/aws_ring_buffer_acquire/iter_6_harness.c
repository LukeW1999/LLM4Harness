#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t init_size = nondet_size_t();
    __CPROVER_assume(init_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(init_size > 0);

    int init_res = aws_ring_buffer_init(&ring_buf, allocator, init_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    size_t old_head = ring_buf.head;
    size_t old_tail = ring_buf.tail;

    struct aws_byte_buf dest = {0};

    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    size_t new_head = ring_buf.head;
    size_t new_tail = ring_buf.tail;

    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer < ring_buf.allocation_end);
        assert((size_t)(ring_buf.allocation_end - (uint8_t *)dest.buffer) >= requested_size);
        assert(new_head != old_head);
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
    } else {
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(new_head == old_head);
        assert(new_tail == old_tail);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
