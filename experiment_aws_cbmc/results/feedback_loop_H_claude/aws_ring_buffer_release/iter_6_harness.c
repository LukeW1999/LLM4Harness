#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    if (aws_ring_buffer_init(&ring_buffer, allocator, size) != AWS_OP_SUCCESS) {
        return;
    }

    /* Set up buf to point within the ring buffer allocation */
    struct aws_byte_buf buf;
    buf.allocator = NULL;
    buf.len = 0;

    /* buf.buffer must be within [allocation, allocation_end) */
    size_t offset;
    __CPROVER_assume(offset < size);
    buf.buffer = ring_buffer.allocation + offset;

    /* buf.capacity must be > 0 and buf.buffer + buf.capacity <= allocation_end */
    size_t remaining = (size_t)(ring_buffer.allocation_end - buf.buffer);
    __CPROVER_assume(buf.capacity > 0 && buf.capacity <= remaining);
    __CPROVER_assume(buf.buffer + buf.capacity <= ring_buffer.allocation_end);

    /* Set tail to buf.buffer so release is valid */
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)buf.buffer);

    /* Set head to buf.buffer + buf.capacity or somewhere valid */
    uint8_t *new_head = buf.buffer + buf.capacity;
    aws_atomic_store_ptr(&ring_buffer.head, (void *)new_head);

    /* Verify validity */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Save old state */
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;

    /* Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Assert postconditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    assert(aws_ring_buffer_is_valid(&ring_buffer));

    aws_ring_buffer_clean_up(&ring_buffer);
}
