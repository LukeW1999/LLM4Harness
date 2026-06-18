#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();
    ring.allocator = allocator;

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;

    AWS_ATOMIC_STORE_HEAD_PTR(&ring, ring.allocation);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring, ring.allocation);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    size_t offset = nondet_size_t();
    size_t cap = nondet_size_t();
    __CPROVER_assume(offset <= ring_size);
    __CPROVER_assume(cap > 0 && cap <= ring_size - offset);
    buf.buffer = ring.allocation + offset;
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_head = AWS_ATOMIC_LOAD_HEAD_PTR(&ring);

    aws_ring_buffer_release(&ring, &buf);

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    assert(AWS_ATOMIC_LOAD_TAIL_PTR(&ring) == old_buf.buffer + old_buf.capacity);
    assert(AWS_ATOMIC_LOAD_HEAD_PTR(&ring) == old_head);
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(aws_ring_buffer_is_valid(&ring));
}
