#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = (size_t)nondet_uint64_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    int init_res = aws_ring_buffer_init(&ring, allocator, size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    size_t req = (size_t)nondet_uint64_t();
    __CPROVER_assume(req > 0 && req <= size);
    int acq_res = aws_ring_buffer_acquire(&ring, req, &buf);
    __CPROVER_assume(acq_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    size_t old_head = ring.head;
    size_t old_tail = ring.tail;
    size_t old_capacity = buf.capacity;

    aws_ring_buffer_release(&ring, &buf);

    __CPROVER_assert(buf.buffer == NULL, "buf.buffer reset");
    __CPROVER_assert(buf.capacity == 0, "buf.capacity reset");
    __CPROVER_assert(buf.len == 0, "buf.len reset");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator reset");

    __CPROVER_assert(ring.allocator == allocator, "ring allocator unchanged");
    __CPROVER_assert(ring.size == size, "ring size unchanged");
    __CPROVER_assert(ring.head == old_head, "ring head unchanged");
    __CPROVER_assert(ring.tail == old_tail + old_capacity, "ring tail advanced");

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring), "ring still valid");
}
