#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_acquire_harness() {
    /* 1. Set up the ring buffer using aws_ring_buffer_init */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t buffer_size;
    __CPROVER_assume(buffer_size > 0 && buffer_size <= MAX_BUFFER_SIZE);

    /* Initialize the ring buffer properly */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, buffer_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set head and tail to nondeterministic valid positions */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= buffer_size);
    __CPROVER_assume(tail_offset <= buffer_size);

    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;

    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Assume the ring buffer is valid after setting head/tail */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up the destination byte buffer */
    struct aws_byte_buf dest;
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* 3. Set up requested_size - must be non-zero per precondition */
    size_t requested_size;
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= buffer_size);

    /* 4. Save old ring buffer state */
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *old_allocator = ring_buf.allocator;

    /* 5. Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Assert postconditions */

    /* Fields that must never change */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest should be a valid byte buffer with the requested size */
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.buffer != NULL);

        /* The buffer must be within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);

        /* The dest buffer must belong to the ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* Ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure: ring buffer should still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* Ring buffer validity must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
