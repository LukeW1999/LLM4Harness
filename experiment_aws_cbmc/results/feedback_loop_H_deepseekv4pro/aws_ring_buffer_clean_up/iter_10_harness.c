#include <aws/common/ring_buffer.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Properly initialize the ring buffer to satisfy aws_ring_buffer_is_valid precondition */
    int result = aws_ring_buffer_init(&ring_buf, allocator, 1024);
    __CPROVER_assume(result == AWS_OP_SUCCESS);

    aws_ring_buffer_clean_up(&ring_buf);
}
