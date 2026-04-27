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

    /* Non-deterministically decide if we have an allocation */
    int has_allocation;
    __CPROVER_assume(has_allocation == 0 || has_allocation == 1);

    if (has_allocation) {
        size_t allocation_size;
        __CPROVER_assume(allocation_size > 0 && allocation_size <= MAX_BUFFER_SIZE);

        uint8_t *allocation = aws_mem_acquire(allocator, allocation_size);
        __CPROVER_assume(allocation != NULL);

        ring_buf.allocator = allocator;
        ring_buf.allocation = allocation;
        ring_buf.allocation_end = allocation + allocation_size;

        aws_atomic_init_ptr(&ring_buf.head, (void *)allocation);
        aws_atomic_init_ptr(&ring_buf.tail, (void *)allocation);
    } else {
        ring_buf.allocator = allocator;
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
        aws_atomic_init_ptr(&ring_buf.head, NULL);
        aws_atomic_init_ptr(&ring_buf.tail, NULL);
    }

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
