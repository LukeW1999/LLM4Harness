#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    struct aws_allocator *allocator = aws_default_allocator();

    size_t ring_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(ring_size > 0);
    ring.allocation = (uint8_t *)aws_mem_acquire(allocator, ring_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;
    ring.allocator = allocator;

    ring.head.value = (void *)ring.allocation;
    ring.tail.value = (void *)ring.allocation;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    aws_ring_buffer_release(&ring, &buf);

    assert((uint8_t *)ring.tail.value == old_buf.buffer + old_buf.capacity);
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert((uint8_t *)ring.head.value == (uint8_t *)old_ring.head.value);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(aws_ring_buffer_is_valid(&ring));
}
