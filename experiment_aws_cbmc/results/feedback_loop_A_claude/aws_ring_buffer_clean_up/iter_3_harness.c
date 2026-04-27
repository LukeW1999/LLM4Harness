#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;

    /* Use a concrete allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    size_t allocation_size;
    __CPROVER_assume(allocation_size > 0 && allocation_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = aws_mem_acquire(allocator, allocation_size);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = allocator;
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + allocation_size;

    /* head and tail must point within [allocation, allocation_end] for validity */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= allocation_size);
    __CPROVER_assume(tail_offset <= allocation_size);

    aws_atomic_init_ptr(&ring_buf.head, (void *)(allocation + head_offset));
    aws_atomic_init_ptr(&ring_buf.tail, (void *)(allocation + tail_offset));

    /* Ensure the ring buffer is valid before calling clean_up */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Assert postconditions: AWS_ZERO_STRUCT zeroes all fields */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* Check that head and tail atomics are zeroed */
    void *head_ptr = aws_atomic_load_ptr(&ring_buf.head);
    void *tail_ptr = aws_atomic_load_ptr(&ring_buf.tail);
    assert(head_ptr == NULL);
    assert(tail_ptr == NULL);
}
