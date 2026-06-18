#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_ring_buffer_clean_up
 *
 * Analysis:
 * 1. Changed fields on success: ALL fields are zeroed (AWS_ZERO_STRUCT(*ring_buf))
 *    - allocator -> NULL
 *    - allocation -> NULL
 *    - head -> 0
 *    - tail -> 0
 *    - allocation_end -> NULL
 * 2. Unchanged fields: none (everything is zeroed)
 * 3. Failure: function is void, no failure path
 * 4. Validity invariants: after clean_up, the struct is zeroed
 */

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare and set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Non-deterministically decide if allocation is NULL or valid */
    if (nondet_bool()) {
        /* Initialize with a real allocator and some size */
        size_t size = nondet_size_t();
        __CPROVER_assume(size > 0 && size <= 1024);
        int init_result = aws_ring_buffer_init(&ring_buf, aws_default_allocator(), size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);
        __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* allocation is NULL */
        ring_buf.allocator = aws_default_allocator();
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
        aws_atomic_init_int(&ring_buf.head, 0);
        aws_atomic_init_int(&ring_buf.tail, 0);
    }

    /* 2. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions: AWS_ZERO_STRUCT zeroes everything */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* After AWS_ZERO_STRUCT, the atomic vars should be zero as well.
     * We check the underlying value via load. */
    size_t head_val = aws_atomic_load_int(&ring_buf.head);
    size_t tail_val = aws_atomic_load_int(&ring_buf.tail);
    assert(head_val == 0);
    assert(tail_val == 0);
}

void aws_ring_buffer_clean_up_harness(void) {
    aws_ring_buffer_clean_up_harness();
    return 0;
}
