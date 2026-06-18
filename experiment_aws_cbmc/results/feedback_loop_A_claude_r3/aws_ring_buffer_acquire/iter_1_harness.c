#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Allocate a backing buffer of bounded size */
    size_t buffer_size;
    __CPROVER_assume(buffer_size > 0 && buffer_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(buffer_size);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buffer_size;

    /* Set up head and tail atomics to valid positions within the buffer */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;

    /* head and tail must be within [allocation, allocation_end] */
    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= buffer_size);
    __CPROVER_assume(tail_offset <= buffer_size);

    head_ptr = allocation + head_offset;
    tail_ptr = allocation + tail_offset;

    aws_atomic_init_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up requested_size - must be non-zero per precondition */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= buffer_size); /* bound state space */

    /* 3. Set up dest buffer */
    struct aws_byte_buf dest;
    /* Initialize dest to some state */
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* 4. Save old ring buffer state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    /* Save old allocator pointer */
    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;

    /* 5. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Assert postconditions */

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
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);

        /* The ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* The buffer belongs to this ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        /* On failure: ring buffer state should be unchanged (head/tail may not have changed) */
        /* The ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
