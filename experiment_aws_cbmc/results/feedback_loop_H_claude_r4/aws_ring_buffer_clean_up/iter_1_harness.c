#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_ring_buffer_clean_up.
 *
 * Analysis:
 * 1. Changed fields on success: all fields are zeroed (AWS_ZERO_STRUCT(*ring_buf))
 *    - ring_buf->allocator becomes NULL
 *    - ring_buf->allocation becomes NULL
 *    - ring_buf->allocation_end becomes NULL
 *    - ring_buf->head and ring_buf->tail are zeroed
 * 2. Unchanged fields: none — everything is zeroed
 * 3. Failure: function is void, no failure path
 * 4. Validity invariants: after clean_up, the struct is zeroed
 */

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare and set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* We need a valid ring buffer to clean up.
     * Use aws_ring_buffer_init to create a valid one, or set up manually. */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Use a bounded size to keep state space manageable */
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);

    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Only proceed if initialization succeeded */
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions:
     * After clean_up, AWS_ZERO_STRUCT zeroes all fields */

    /* allocation should be NULL after clean_up */
    assert(ring_buf.allocation == NULL);

    /* allocator should be NULL after clean_up */
    assert(ring_buf.allocator == NULL);

    /* allocation_end should be NULL after clean_up */
    assert(ring_buf.allocation_end == NULL);

    /* head and tail atomics should be zeroed.
     * We check by loading their values — they should be 0 (NULL) */
    size_t head_val = aws_atomic_load_int(&ring_buf.head);
    size_t tail_val = aws_atomic_load_int(&ring_buf.tail);
    assert(head_val == 0);
    assert(tail_val == 0);
}
