#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up the byte buf with nondeterministic buffer pointer */
    struct aws_byte_buf buf;
    buf.len = 0;
    buf.capacity = 0;
    buf.allocator = NULL;
    buf.buffer = (uint8_t *)nondet_voidp();

    /* 3. Snapshot state before the call */
    struct aws_allocator *old_rb_allocator = ring_buf.allocator;
    uint8_t *old_rb_allocation = ring_buf.allocation;
    uint8_t *old_rb_allocation_end = ring_buf.allocation_end;

    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    struct aws_allocator *old_buf_allocator = buf.allocator;

    /* 4. Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* 5. Assert postconditions */
    if (result) {
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer < ring_buf.allocation_end);
    }

    /* FRAME: ring_buffer must not be modified */
    assert(ring_buf.allocator == old_rb_allocator);
    assert(ring_buf.allocation == old_rb_allocation);
    assert(ring_buf.allocation_end == old_rb_allocation_end);

    /* FRAME: buf must not be modified */
    assert(buf.buffer == old_buf_buffer);
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.allocator == old_buf_allocator);

    /* INVARIANTS */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
