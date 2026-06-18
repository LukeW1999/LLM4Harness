#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buf;

    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up byte buf non-deterministically */
    struct aws_byte_buf buf;
    buf.len = 0;
    buf.capacity = 0;
    buf.allocator = NULL;

    uint8_t *nondet_ptr;
    buf.buffer = nondet_ptr;

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    int result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    if (result) {
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer < ring_buf.allocation_end);
    }

    /* Unchanged fields of ring_buf */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* Unchanged fields of buf */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
