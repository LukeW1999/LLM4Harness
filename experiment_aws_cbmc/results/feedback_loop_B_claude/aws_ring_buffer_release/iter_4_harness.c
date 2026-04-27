#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;

    /* Use a fixed-size static buffer to avoid malloc issues */
    static uint8_t backing[MAX_BUFFER_SIZE];
    size_t alloc_size = MAX_BUFFER_SIZE;

    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = backing;
    ring_buffer.allocation_end = backing + alloc_size;

    /* Set head and tail atomics to valid pointers within allocation */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= alloc_size);
    __CPROVER_assume(tail_offset <= alloc_size);

    aws_atomic_init_ptr(&ring_buffer.head, (void *)(backing + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(backing + tail_offset));

    /* Set up buf - buffer and buffer+capacity must be within allocation */
    struct aws_byte_buf buf;

    size_t buf_offset;
    size_t buf_capacity;
    __CPROVER_assume(buf_offset < alloc_size);
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= alloc_size - buf_offset);

    buf.buffer = backing + buf_offset;
    buf.capacity = buf_capacity;
    buf.len = 0;
    buf.allocator = NULL;

    /* Save expected new tail before call */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* Save old ring_buffer fields */
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);

    /* Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Assert postconditions */

    /* buf should be zeroed after the call */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* ring_buffer->tail should be updated to old buf->buffer + buf->capacity */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_new_tail);

    /* ring_buffer fields that should NOT change */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    /* head should not change */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);
}
