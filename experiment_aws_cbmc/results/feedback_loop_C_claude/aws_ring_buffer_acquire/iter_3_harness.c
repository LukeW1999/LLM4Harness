#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#ifndef MAX_RING_BUFFER_SIZE
#    define MAX_RING_BUFFER_SIZE 16
#endif

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;

    /* Use a concrete allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Allocate a concrete buffer for the ring buffer */
    size_t buf_size;
    __CPROVER_assume(buf_size > 1 && buf_size <= MAX_RING_BUFFER_SIZE);

    /* Initialize the ring buffer properly using the API */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, buf_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Verify the ring buffer is valid after init */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set head and tail to nondeterministic valid values within the allocation */
    uint8_t *allocation = ring_buf.allocation;
    uint8_t *allocation_end = ring_buf.allocation_end;
    size_t actual_buf_size = allocation_end - allocation;

    uint8_t *head_ptr;
    uint8_t *tail_ptr;
    __CPROVER_assume(head_ptr >= allocation && head_ptr <= allocation_end);
    __CPROVER_assume(tail_ptr >= allocation && tail_ptr <= allocation_end);

    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Ensure the ring buffer is valid with these head/tail values */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up requested_size */
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_RING_BUFFER_SIZE);

    /* Save old head and tail */
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    /* Set up dest buffer */
    struct aws_byte_buf dest;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    /* Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Frame conditions: allocation pointers never change */
    assert(ring_buf.allocation == allocation);
    assert(ring_buf.allocation_end == allocation_end);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest must be valid and point within ring buffer */
        assert(dest.capacity >= requested_size);
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);

    } else {
        /* On failure: ring buffer head must be unchanged */
        uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        assert(new_head == old_head);
    }

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
