#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    struct aws_allocator *allocator = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= 1024);
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;
    ring_buffer.allocator = allocator;

    AWS_ATOMIC_STORE_PTR(&ring_buffer.head, ring_buffer.allocation);
    AWS_ATOMIC_STORE_PTR(&ring_buffer.tail, ring_buffer.allocation);

    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= ring_size);
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= ring_size - offset);
    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= cap);
    buf.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_head = AWS_ATOMIC_LOAD_PTR(&ring_buffer.head);
    uint8_t *old_tail = AWS_ATOMIC_LOAD_PTR(&ring_buffer.tail);

    aws_ring_buffer_release(&ring_buffer, &buf);

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(AWS_ATOMIC_LOAD_PTR(&ring_buffer.head) == old_head);
    assert(AWS_ATOMIC_LOAD_PTR(&ring_buffer.tail) == old_tail + old_buf.len);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
