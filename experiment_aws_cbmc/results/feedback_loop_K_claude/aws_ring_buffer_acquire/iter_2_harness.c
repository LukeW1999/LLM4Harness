#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum allocation size for the ring buffer in the proof */
#define MAX_RING_BUFFER_SIZE 16

void aws_ring_buffer_acquire_harness(void) {
    /* Allocate and initialize the ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;

    /* Choose a nondet size for the ring buffer, bounded for tractability */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_RING_BUFFER_SIZE);

    /* Initialize the ring buffer using the real init function */
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Set up nondet head and tail pointers within valid bounds */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= ring_size);
    __CPROVER_assume(tail_offset <= ring_size);

    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;

    /* Store nondet head/tail into the ring buffer atomics */
    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Verify the ring buffer is still valid after setting head/tail */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Choose a nondet requested_size, bounded and nonzero */
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_RING_BUFFER_SIZE);

    /* Save pre-call state for frame conditions */
    struct aws_allocator *pre_allocator = ring_buf.allocator;
    uint8_t *pre_allocation = ring_buf.allocation;
    uint8_t *pre_allocation_end = ring_buf.allocation_end;

    /* Call the function under verification */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postconditions: return value is either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* dest.buffer must be non-null */
        assert(dest.buffer != NULL);

        /* dest.len and dest.capacity must equal requested_size */
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);

        /* dest.buffer must be within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);

        /* The acquired buffer must belong to the ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    }

    /* Frame conditions: allocator, allocation, allocation_end must not change */
    assert(ring_buf.allocator == pre_allocator);
    assert(ring_buf.allocation == pre_allocation);
    assert(ring_buf.allocation_end == pre_allocation_end);

    /* Ring buffer must remain valid after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
