aws_ring_buffer_buf_belongs_to_pool(const struct aws_ring_buffer *ring_buffer, const struct aws_byte_buf *buf) {
    bool rval = s_buf_belongs_to_pool(ring_buffer, buf);
    return rval;
}