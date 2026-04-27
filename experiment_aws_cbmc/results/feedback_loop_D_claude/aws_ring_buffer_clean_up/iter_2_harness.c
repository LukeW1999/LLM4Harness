#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* Allocate a ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Set up allocator - use a nondeterministic valid allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    /* allocation must be non-NULL for aws_ring_buffer_is_valid to hold */
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);
    ring_buf.allocation = malloc(size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + size;

    /* Set head and tail to values within the valid range */
    /* head and tail must be within [allocation, allocation_end] for validity */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= size);
    __CPROVER_assume(tail_offset <= size);
    aws_atomic_store_ptr(&ring_buf.head, (void *)(ring_buf.allocation + head_offset));
    aws_atomic_store_ptr(&ring_buf.tail, (void *)(ring_buf.allocation + tail_offset));

    /* Ensure the ring buffer is valid before calling clean_up */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* === ASSERTIONS: After clean_up, the entire struct should be zeroed === */

    /* allocator should be NULL */
    assert(ring_buf.allocator == NULL);

    /* allocation should be NULL */
    assert(ring_buf.allocation == NULL);

    /* allocation_end should be NULL */
    assert(ring_buf.allocation_end == NULL);

    /* head should be zeroed (atomic value should be 0/NULL) */
    void *head_val = aws_atomic_load_ptr(&ring_buf.head);
    assert(head_val == NULL);

    /* tail should be zeroed (atomic value should be 0/NULL) */
    void *tail_val = aws_atomic_load_ptr(&ring_buf.tail);
    assert(tail_val == NULL);
}
