#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 4096

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buffer.allocator = allocator;

    /* Allocate backing memory for ring buffer */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    /* Choose a valid buffer location and capacity within the allocation */
    size_t offset;
    size_t buf_capacity;
    __CPROVER_assume(offset <= ring_size);
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(offset + buf_capacity <= ring_size);

    buf.allocator = allocator;
    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = buf_capacity;
    buf.len = 0;
    /* buf.len can be any value up to capacity, here we use 0 for simplicity.
       but can also be nondeterministic with assume(buf.len <= buf.capacity) */
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Set ring buffer head and tail to match this single acquired buffer */
    uint8_t *head_ptr = buf.buffer;
    uint8_t *tail_ptr = buf.buffer + buf.capacity;

    aws_atomic_store_ptr(&ring_buffer.head, head_ptr);
    aws_atomic_store_ptr(&ring_buffer.tail, tail_ptr);

    /* Ensure all invariants hold */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));

    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* head must not change */
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.head);
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);
}
