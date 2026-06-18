#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buf;

    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    if (aws_ring_buffer_init(&ring_buf, allocator, size) != AWS_OP_SUCCESS) {
        return;
    }
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up byte buf non-deterministically */
    struct aws_byte_buf buf;
    buf.len = 0;
    buf.capacity = 0;
    buf.allocator = NULL;

    uint8_t *nondet_ptr;
    buf.buffer = nondet_ptr;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* Check that ring_buffer fields are unchanged */
    __CPROVER_assert(ring_buf.allocator == allocator, "allocator unchanged");
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf), "ring buffer valid");

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
