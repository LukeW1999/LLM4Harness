#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare and set up a valid ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize with a valid allocator and some size */
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);

    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions:
     *
     * From the implementation:
     *   - aws_mem_release is called on ring_buf->allocation if non-null
     *   - AWS_ZERO_STRUCT(*ring_buf) is called, zeroing all fields
     *
     * After clean_up:
     *   - ring_buf.allocator should be NULL (zeroed)
     *   - ring_buf.allocation should be NULL (zeroed)
     *   - ring_buf.allocation_end should be NULL (zeroed)
     *   - ring_buf.head atomic var should be zeroed
     *   - ring_buf.tail atomic var should be zeroed
     */

    /* All fields should be zeroed after AWS_ZERO_STRUCT */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* The head and tail atomics should be zeroed.
     * We check the underlying value stored in the atomic.
     * aws_atomic_var stores a size_t value internally. */
    size_t head_val = aws_atomic_load_int(&ring_buf.head);
    size_t tail_val = aws_atomic_load_int(&ring_buf.tail);
    assert(head_val == 0);
    assert(tail_val == 0);
}
