#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();
    ring.allocator = allocator;

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)__CPROVER_allocate(ring_size, 0);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;

    AWS_ATOMIC_STORE_HEAD_PTR(&ring, ring.allocation);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring, ring.allocation);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* The buffer to be released must start at the current tail */
    size_t offset = 0;
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap > 0 && cap <= ring_size - offset);
    buf.buffer = ring.allocation + offset;
    buf.capacity = cap;
    buf.len = 0; /* releasing an empty buffer */
    buf.allocator = allocator;

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_head = AWS_ATOMIC_LOAD_HEAD_PTR(&ring);
    uint8_t *old_tail = AWS_ATOMIC_LOAD_TAIL_PTR(&ring);

    aws_ring_buffer_release(&ring, &buf);

    /* Buffer should be cleared */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Tail should have advanced by the released capacity */
    assert(AWS_ATOMIC_LOAD_TAIL_PTR(&ring) == old_tail + old_buf.capacity);
    /* Head should remain unchanged */
    assert(AWS_ATOMIC_LOAD_HEAD_PTR(&ring) == old_head);
    /* Ring allocator and allocation pointers remain unchanged */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    /* Ring remains valid */
    assert(aws_ring_buffer_is_valid(&ring));
}
