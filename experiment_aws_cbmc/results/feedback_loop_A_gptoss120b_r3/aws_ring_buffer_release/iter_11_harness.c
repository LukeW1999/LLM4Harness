#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring = {0};
    struct aws_byte_buf buf = {0};

    struct aws_allocator *allocator = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = (uint8_t *)aws_mem_acquire(allocator, ring_size);
    __CPROVER_assume(allocation != NULL);
    ring.allocation = allocation;
    ring.allocation_end = allocation + ring_size;
    ring.allocator = allocator;
    ring.head = 0;
    ring.tail = 0;
    ring.size = ring_size;

    ensure_byte_buf_has_allocated_buffer_member(&buf);

    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= ring_size);
    buf.buffer = ring.allocation + offset;

    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= ring_size - offset);
    buf.capacity = cap;

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= cap);
    buf.len = len;

    buf.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring, &buf));

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    aws_ring_buffer_release(&ring, &buf);

    size_t expected_tail = old_ring.tail + old_buf.capacity;
    if (expected_tail >= old_ring.size) {
        expected_tail -= old_ring.size;
    }
    assert(ring.tail == expected_tail);

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.size == old_ring.size);

    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
