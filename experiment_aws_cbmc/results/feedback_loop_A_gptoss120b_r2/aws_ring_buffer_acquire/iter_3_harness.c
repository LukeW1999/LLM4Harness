#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t init_size = nondet_size_t();
    __CPROVER_assume(init_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(init_size > 0);

    int init_res = aws_ring_buffer_init(&ring_buf, allocator, init_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_ring_buffer old_ring = ring_buf;
    uint8_t *old_head = NULL;
    uint8_t *old_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail);

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    struct aws_byte_buf dest;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        uint8_t *new_head = NULL;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        assert(new_head != old_head);

        uint8_t *new_tail = NULL;
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        assert(new_tail == old_tail);
    } else {
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);

        uint8_t *new_head = NULL;
        uint8_t *new_tail = NULL;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        assert(new_head == old_head);
        assert(new_tail == old_tail);
    }

    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
