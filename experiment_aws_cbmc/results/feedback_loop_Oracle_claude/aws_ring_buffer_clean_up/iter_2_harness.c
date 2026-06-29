#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;

    /* Non-deterministic initialization */
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the ring buffer properly */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions */

    /* After clean up, the ring buffer should be zeroed out */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* The atomic head and tail should be zero after AWS_ZERO_STRUCT */
    size_t head_val = aws_atomic_load_int(&ring_buf.head);
    size_t tail_val = aws_atomic_load_int(&ring_buf.tail);
    assert(head_val == 0);
    assert(tail_val == 0);
}
