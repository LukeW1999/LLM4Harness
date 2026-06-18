#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness(void) {
    /* Stack‑allocated structures under test */
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    /* Allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate and initialise the internal ring buffer storage */
    size_t ring_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(ring_size > 0);
    ring.allocation = (uint8_t *)aws_mem_acquire(allocator, ring_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;
    ring.allocator = allocator;

    /* Initialise atomic head and tail */
    ring.head.value = (void *)ring.allocation;
    ring.tail.value = (void *)ring.allocation;

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* Prepare a byte buffer with an allocated backing store */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Save the pre‑state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* Post‑conditions */
    assert((uint8_t *)ring.tail.value == old_buf.buffer + old_buf.capacity);
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert((uint8_t *)ring.head.value == (uint8_t *)old_ring.head.value);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Ring buffer must remain valid */
    assert(aws_ring_buffer_is_valid(&ring));
}
