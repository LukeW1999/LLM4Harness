#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* We need to define bounds for the ring buffer allocation */
#ifndef MAX_RING_BUFFER_SIZE
#    define MAX_RING_BUFFER_SIZE 16
#endif

/*
 * Helper: set up a valid ring_buffer with a real allocation backing it.
 * We do this manually since aws_ring_buffer_init calls malloc.
 */
static void ensure_ring_buffer_has_allocated_buffer(struct aws_ring_buffer *ring_buf, size_t size) {
    /* Allocate a backing array */
    uint8_t *allocation = malloc(size);
    __CPROVER_assume(allocation != NULL);
    ring_buf->allocation = allocation;
    ring_buf->allocation_end = allocation + size;
}

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Choose a bounded buffer size */
    size_t buf_size;
    __CPROVER_assume(buf_size > 0 && buf_size <= MAX_RING_BUFFER_SIZE);

    /* Set up allocator (can be non-det pointer, we won't call alloc in this function) */
    ring_buf.allocator = nondet_bool() ? NULL : malloc(sizeof(struct aws_allocator));

    /* Set up the backing allocation */
    ensure_ring_buffer_has_allocated_buffer(&ring_buf, buf_size);

    /* Set up head and tail atomics non-deterministically within valid range */
    uint8_t *head_val;
    uint8_t *tail_val;

    /* head and tail must be within [allocation, allocation_end] */
    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= buf_size);
    __CPROVER_assume(tail_offset <= buf_size);

    head_val = ring_buf.allocation + head_offset;
    tail_val = ring_buf.allocation + tail_offset;

    aws_atomic_init_ptr(&ring_buf.head, (void *)head_val);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)tail_val);

    /* 2. Set up the destination buffer */
    struct aws_byte_buf dest;
    /* Initialize dest to known state */
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* 3. Choose a non-zero requested_size (precondition from AWS_ERROR_PRECONDITION) */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_RING_BUFFER_SIZE);

    /* 4. Save old state of ring_buf fields that should not change */
    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;

    /* Load head/tail before call for comparison */
    uint8_t *old_head;
    uint8_t *old_tail;
    old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    /* 5. Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Load new head/tail after call */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    /* 7. Assert postconditions */

    /* Fields that must NEVER change regardless of result */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - dest.buffer must point within [allocation, allocation_end)
         * - dest.len == 0 (from_empty_array sets len=0)
         * - dest.capacity == requested_size
         * - new_head == dest.buffer + requested_size (or wrapped)
         * - dest.buffer is within the ring buffer allocation
         */
        assert(dest.buffer != NULL);
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);

        /* dest.buffer must be within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer < ring_buf.allocation_end);

        /* new_head should be dest.buffer + requested_size */
        assert(new_head == dest.buffer + requested_size);

        /* new_head must be within [allocation, allocation_end] */
        assert(new_head >= ring_buf.allocation);
        assert(new_head <= ring_buf.allocation_end);

        /* The dest buffer must be valid */
        assert(aws_byte_buf_is_valid(&dest));

    } else {
        /* On failure (AWS_ERROR_OOM):
         * - dest should not have been written (or at least ring_buf unchanged)
         * - head and tail should be unchanged
         */
        assert(result == AWS_OP_ERR);

        /* On failure, head and tail are not modified */
        assert(new_head == old_head);
        assert(new_tail == old_tail);
    }
}
