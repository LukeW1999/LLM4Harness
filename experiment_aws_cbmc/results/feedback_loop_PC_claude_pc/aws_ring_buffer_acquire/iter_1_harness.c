#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Allocate a backing buffer of bounded size */
    size_t buf_size;
    __CPROVER_assume(buf_size > 0 && buf_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(buf_size);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buf_size;

    /* Set head and tail to valid positions within the allocation */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;

    /* head and tail must be within [allocation, allocation_end] */
    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= buf_size);
    __CPROVER_assume(tail_offset <= buf_size);

    head_ptr = allocation + head_offset;
    tail_ptr = allocation + tail_offset;

    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up the requested size */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0); /* precondition from AWS_ERROR_PRECONDITION */
    __CPROVER_assume(requested_size <= buf_size); /* bound for tractability */

    /* 3. Set up the destination buffer */
    struct aws_byte_buf dest;
    /* Initialize dest to some known state */
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* 4. Snapshot ring buffer state before call */
    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;

    /* Load head/tail before call for comparison */
    uint8_t *head_before = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *tail_before = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    /* 5. Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Assert postconditions */

    /* Frame: allocator, allocation, allocation_end must not change */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest must be valid and have the requested size */
        assert(dest.len == 0); /* aws_byte_buf_from_empty_array sets len=0 */
        assert(dest.capacity == requested_size);
        assert(dest.buffer != NULL);

        /* dest.buffer must be within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);

        /* The ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* The dest buffer belongs to the ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        /* Head must have advanced by requested_size (or wrapped) */
        uint8_t *head_after = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        assert(head_after == dest.buffer + requested_size);

        /* Tail must point to dest.buffer (set when head==tail initially, or unchanged) */
        uint8_t *tail_after = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

        /* If head_before == tail_before (empty buffer case), tail is set to allocation */
        if (head_before == tail_before) {
            assert(tail_after == ring_buf.allocation);
            assert(dest.buffer == ring_buf.allocation);
        }

    } else {
        /* On failure: dest should not have been modified in a meaningful way,
           and the ring buffer state should be consistent */
        /* The ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* Head and tail should be unchanged on failure */
        uint8_t *head_after = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_after = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_after == head_before);
        assert(tail_after == tail_before);
    }
}
