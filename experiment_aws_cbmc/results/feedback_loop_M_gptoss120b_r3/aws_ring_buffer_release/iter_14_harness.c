#include <assert.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();
    ring.allocator = allocator;

    size_t ring_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)__CPROVER_allocate(ring_size, 0);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;
    ring.allocation_size = ring_size;

    aws_atomic_store_ptr(&ring.head, ring.allocation);
    aws_atomic_store_ptr(&ring.tail, ring.allocation);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    size_t offset = (size_t)nondet_uint64_t();
    __CPROVER_assume(offset < ring_size);
    size_t cap = (size_t)nondet_uint64_t();
    __CPROVER_assume(cap > 0 && cap <= ring_size - offset);
    buf.buffer = ring.allocation + offset;
    buf.capacity = cap;
    buf.len = 0;
    buf.allocator = NULL;

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_head = aws_atomic_load_ptr(&ring.head);
    uint8_t *old_tail = aws_atomic_load_ptr(&ring.tail);

    aws_ring_buffer_release(&ring, &buf);

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    assert(aws_atomic_load_ptr(&ring.tail) == old_tail + old_buf.capacity);
    assert(aws_atomic_load_ptr(&ring.head) == old_head);
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.allocation_size == old_ring.allocation_size);
    assert(aws_ring_buffer_is_valid(&ring));
}
