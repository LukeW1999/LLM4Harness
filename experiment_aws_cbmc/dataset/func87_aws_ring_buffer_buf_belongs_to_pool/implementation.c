aws_ring_buffer_buf_belongs_to_pool(const struct aws_ring_buffer *ring_buffer, const struct aws_byte_buf *buf) {
    AWS_PRECONDITION(aws_ring_buffer_is_valid(ring_buffer));
    AWS_PRECONDITION(aws_byte_buf_is_valid(buf));
    bool rval = s_buf_belongs_to_pool(ring_buffer, buf);
    AWS_POSTCONDITION(aws_ring_buffer_is_valid(ring_buffer));
    AWS_POSTCONDITION(aws_byte_buf_is_valid(buf));
    return rval;
}