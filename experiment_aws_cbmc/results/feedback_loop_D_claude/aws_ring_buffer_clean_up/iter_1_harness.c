// === STEP 1: SUCCESS PATH ===
// aws_ring_buffer_clean_up is void, no return value.
// After the call:
//   - ring_buf->allocation: CHANGES to NULL (zeroed by AWS_ZERO_STRUCT)
//   - ring_buf->allocator: CHANGES to NULL (zeroed by AWS_ZERO_STRUCT)
//   - ring_buf->head: CHANGES to 0 (zeroed by AWS_ZERO_STRUCT)
//   - ring_buf->tail: CHANGES to 0 (zeroed by AWS_ZERO_STRUCT)
//   - ring_buf->allocation_end: CHANGES to NULL (zeroed by AWS_ZERO_STRUCT)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always completes.
//
// === STEP 3: FRAME CONDITIONS ===
// ring_buf (struct aws_ring_buffer):
//   - allocator: CHANGED (zeroed to NULL)
//   - allocation: CHANGED (zeroed to NULL)
//   - head: CHANGED (zeroed to 0)
//   - tail: CHANGED (zeroed to 0)
//   - allocation_end: CHANGED (zeroed to NULL)
//
// === STEP 4: VALIDITY INVARIANTS ===
// After AWS_ZERO_STRUCT(*ring_buf), the struct is zeroed.
// aws_ring_buffer_is_valid checks that allocation != NULL and allocation <= allocation_end,
// so after clean_up, the ring_buf is NOT valid in the normal sense (allocation is NULL).
// We assert the zeroed state instead.

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

    /* Nondeterministically decide if allocation is NULL or non-NULL */
    if (nondet_bool()) {
        /* allocation is NULL - the if-branch in clean_up won't call aws_mem_release */
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    } else {
        /* allocation is non-NULL - allocate some memory */
        size_t size;
        __CPROVER_assume(size > 0 && size <= 1024);
        ring_buf.allocation = malloc(size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + size;
    }

    /* Set head and tail to some nondeterministic values */
    aws_atomic_store_ptr(&ring_buf.head, (void *)(uintptr_t)nondet_size_t());
    aws_atomic_store_ptr(&ring_buf.tail, (void *)(uintptr_t)nondet_size_t());

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
