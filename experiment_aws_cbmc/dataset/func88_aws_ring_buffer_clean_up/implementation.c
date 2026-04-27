aws_ring_buffer_clean_up(struct aws_ring_buffer *ring_buf) {
    AWS_PRECONDITION(aws_ring_buffer_is_valid(ring_buf));
    if (ring_buf->allocation) {
        aws_mem_release(ring_buf->allocator, ring_buf->allocation);
    }

    AWS_ZERO_STRUCT(*ring_buf);
}