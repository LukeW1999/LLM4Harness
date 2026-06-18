#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare ring buffer */
    struct aws_ring_buffer ring_buf;

    /* allocator must be a valid allocator */
    ring_buf.allocator = aws_default_allocator();

    /* nondet allocation size, bounded */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    /* nondet whether allocation is NULL or a valid buffer */
    bool has_allocation = nondet_bool();
    if (has_allocation) {
        ring_buf.allocation = (uint8_t *)malloc(alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    /* nondet head and tail values, bounded by allocation size if allocation exists */
    size_t head_val = nondet_size_t();
    size_t tail_val = nondet_size_t();
    if (ring_buf.allocation) {
        __CPROVER_assume(head_val <= alloc_size);
        __CPROVER_assume(tail_val <= alloc_size);
    } else {
        __CPROVER_assume(head_val == 0);
        __CPROVER_assume(tail_val == 0);
    }
    aws_atomic_store_int(&ring_buf.head, (int)head_val);
    aws_atomic_store_int(&ring_buf.tail, (int)tail_val);

    /* 2. Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 3. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 4. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 5. Postconditions: all fields must be zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);

    /* 6. The struct should now be a zeroed (empty) ring buffer */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);

    /* 7. Validity invariant should hold for the zeroed structure */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
