#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;

    /* Precondition: use a valid allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Non-deterministically choose whether allocation is NULL or valid */
    bool has_allocation;
    __CPROVER_assume(has_allocation == 0 || has_allocation == 1);

    if (has_allocation) {
        /* Choose a non-deterministic size for the allocation, bounded for tractability */
        size_t size;
        __CPROVER_assume(size > 0 && size <= 1024);

        /* Initialize the ring buffer properly so it is valid */
        int rc = aws_ring_buffer_init(&ring_buf, allocator, size);
        /* Only proceed if initialization succeeded */
        __CPROVER_assume(rc == AWS_OP_SUCCESS);
        /* Precondition: ring buffer must be valid */
        __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* Set up a ring buffer with NULL allocation */
        AWS_ZERO_STRUCT(ring_buf);
        ring_buf.allocator = allocator;
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions: struct is fully zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* Postconditions: atomic head and tail are zeroed (value should be 0) */
    size_t head_val = aws_atomic_load_int(&ring_buf.head);
    size_t tail_val = aws_atomic_load_int(&ring_buf.tail);
    assert(head_val == 0);
    assert(tail_val == 0);
}
