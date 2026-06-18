#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buffer;

    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up byte buf with nondet buffer pointer */
    struct aws_byte_buf buf;
    uint8_t *nondet_ptr;
    buf.buffer = nondet_ptr;

    size_t buf_len;
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;
    buf.capacity = buf_capacity;
    buf.allocator = NULL;

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* If result is true, buf.buffer must be within the ring buffer's allocation range */
    if (result) {
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    }

    /* Ring buffer fields must be unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* buf fields must be unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
