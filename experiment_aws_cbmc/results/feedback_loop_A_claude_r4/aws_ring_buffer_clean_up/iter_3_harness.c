#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare and set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize with a real allocator and some size */
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions: AWS_ZERO_STRUCT zeroes everything */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
