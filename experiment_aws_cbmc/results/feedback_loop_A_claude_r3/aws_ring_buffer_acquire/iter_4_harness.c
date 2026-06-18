#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 16
#endif

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;

    size_t buffer_size;
    __CPROVER_assume(buffer_size >= 2 && buffer_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();

    int init_result = aws_ring_buffer_init(&ring_buf, allocator, buffer_size);
    if (init_result != AWS_OP_SUCCESS) {
        return;
    }

    /* Nondeterministically set head and tail to valid positions within allocation */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset < buffer_size);
    __CPROVER_assume(tail_offset < buffer_size);

    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;

    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Only proceed if the ring buffer is valid after our modifications */
    if (!aws_ring_buffer_is_valid(&ring_buf)) {
        aws_ring_buffer_clean_up(&ring_buf);
        return;
    }

    /* Set up requested_size */
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size < buffer_size);

    /* Set up dest buffer */
    struct aws_byte_buf dest;
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* Save old ring buffer state */
    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;

    /* Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Assert postconditions */

    /* Fields that must never change */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest should be a valid byte buffer with the requested size */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);

        /* dest.buffer must be within the ring buffer's allocation */
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);

        /* The ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* The buffer belongs to this ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        /* On failure: the ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* Cleanup */
    aws_ring_buffer_clean_up(&ring_buf);
}
