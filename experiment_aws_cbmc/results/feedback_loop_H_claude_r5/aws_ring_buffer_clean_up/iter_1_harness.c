#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_ring_buffer_clean_up
 *
 * Analysis:
 * 1. Changed fields on success: all fields zeroed (AWS_ZERO_STRUCT)
 * 2. Unchanged fields: none — everything is zeroed
 * 3. Failure: function is void, no failure path
 * 4. Validity invariants: after clean_up, the struct is zeroed
 *    (aws_ring_buffer_is_valid may not hold on zeroed struct, but
 *     we can assert the fields are zero)
 */

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare and set up ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize with a real allocator and some size */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= 1024); /* bound the size */

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions:
     * AWS_ZERO_STRUCT zeroes all fields, so:
     * - allocator should be NULL
     * - allocation should be NULL
     * - allocation_end should be NULL
     * - head atomic should be zeroed
     * - tail atomic should be zeroed
     */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* The atomic vars should be zeroed as well.
     * We can load them and check they are 0. */
    size_t head_val = aws_atomic_load_int(&ring_buf.head);
    size_t tail_val = aws_atomic_load_int(&ring_buf.tail);
    assert(head_val == 0);
    assert(tail_val == 0);
}
