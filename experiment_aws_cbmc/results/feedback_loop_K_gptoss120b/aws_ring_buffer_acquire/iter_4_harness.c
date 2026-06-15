#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t buf_size = nondet_size_t();
    __CPROVER_assume(buf_size > 0);

    int init_ret = aws_ring_buffer_init(&ring_buf, allocator, buf_size);
    __CPROVER_assume(init_ret == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    struct aws_ring_buffer ring_buf_before = ring_buf;
    struct aws_byte_buf dest_before = dest;

    int ret = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* The function may return success, OOM, or other error codes. */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    if (ret == AWS_OP_SUCCESS) {
        /* On success, the acquired buffer has zero length and the requested capacity. */
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(ring_buf.allocation != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
    } else {
        /* On failure, dest must be unchanged. */
        assert(dest.len == dest_before.len);
        assert(dest.capacity == dest_before.capacity);
        assert(dest.buffer == dest_before.buffer);
    }

    /* The ring buffer's allocation metadata must remain unchanged. */
    assert(ring_buf.allocation == ring_buf_before.allocation);
    assert(ring_buf.allocation_end == ring_buf_before.allocation_end);
    assert(ring_buf.allocator == ring_buf_before.allocator);
}
