#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_release_harness(void) {
    /* Allocate a buffer for the ring buffer's storage on the stack */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    uint8_t ring_storage[MAX_BUFFER_SIZE];
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    struct aws_allocator *allocator = aws_default_allocator();

    ring_buffer.allocation = ring_storage;
    ring_buffer.allocation_end = ring_storage + ring_size;
    ring_buffer.allocator = allocator;

    /* Initialize atomic head and tail */
    uint8_t *head_ptr = ring_storage;
    uint8_t *tail_ptr = ring_storage;
    aws_atomic_store_ptr(&ring_buffer.head, head_ptr);
    aws_atomic_store_ptr(&ring_buffer.tail, tail_ptr);

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up a byte buffer that belongs to the ring buffer pool */
    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= ring_size);
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset + buf_capacity <= ring_size);
    buf.buffer = ring_storage + offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;

    /* Assume the buffer indeed belongs to the pool */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions */
    assert(aws_atomic_load_ptr(&ring_buffer.tail) == old_buf.buffer + old_buf.capacity);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer.head) == aws_atomic_load_ptr(&old_ring.head));
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
