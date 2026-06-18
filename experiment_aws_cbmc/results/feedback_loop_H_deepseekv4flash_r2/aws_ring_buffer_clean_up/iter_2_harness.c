#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old_ring_buf;

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Non-deterministic size for initialization */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0);

    /* Initialize the ring buffer */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Save old state */
    old_ring_buf = ring_buf;

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions: */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
