#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and set up a valid ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize with a valid allocator and some size */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= 1024);

    /* Use aws_ring_buffer_init to get a valid ring buffer */
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions:
     * After clean_up, AWS_ZERO_STRUCT zeroes out the entire structure.
     * All fields should be zero/NULL.
     */

    /* Changed fields: everything is zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* The atomic vars head and tail should also be zeroed.
     * We check them via aws_atomic_load_int which should return 0. */
    size_t head_val = aws_atomic_load_int(&ring_buf.head);
    size_t tail_val = aws_atomic_load_int(&ring_buf.tail);
    assert(head_val == 0);
    assert(tail_val == 0);
}
