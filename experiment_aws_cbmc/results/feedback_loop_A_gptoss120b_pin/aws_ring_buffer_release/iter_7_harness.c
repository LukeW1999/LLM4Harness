#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness(void) {
    /* Declare data structures on the stack */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Allocate and bound the ring buffer's underlying memory */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= 1024);
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    /* Set allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    ring_buffer.allocator = allocator;

    /* Choose a tail offset inside the allocation */
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(tail_offset < ring_size);
    uint8_t *tail_ptr = ring_buffer.allocation + tail_offset;
    aws_atomic_store_ptr(&ring_buffer.tail, tail_ptr);

    /* Prepare a byte buffer that belongs to the ring buffer */
    size_t max_cap = ring_buffer.allocation_end - tail_ptr;
    __CPROVER_assume(max_cap > 0);
    buf.buffer = tail_ptr;
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity > 0 && buf.capacity <= max_cap);
    buf.len = 0;
    buf.allocator = allocator;

    /* Ensure the buffer is recognized as belonging to the pool */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Set head pointer ahead of tail by exactly the buffer capacity (no wrap) */
    uint8_t *head_ptr = tail_ptr + buf.capacity;
    __CPROVER_assume(head_ptr <= ring_buffer.allocation_end);
    aws_atomic_store_ptr(&ring_buffer.head, head_ptr);

    /* Assume the ring buffer is valid with these pointers */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    void *old_head = aws_atomic_load_ptr(&old_ring.head);
    void *old_tail = aws_atomic_load_ptr(&old_ring.tail);

    /* Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Tail pointer should be updated to point to the end of the released buffer */
    assert(aws_atomic_load_ptr(&ring_buffer.tail) == (void *)(old_buf.buffer + old_buf.capacity));

    /* The released byte buffer should be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Unchanged fields of the ring buffer */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer.head) == old_head);
    /* Tail was expected to change, so we do not assert equality with old_tail */

    /* Validity invariant must still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    /* Clean up */
    free(ring_buffer.allocation);
}
