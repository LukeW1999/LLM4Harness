aws_ring_buffer_release(struct aws_ring_buffer *ring_buffer, struct aws_byte_buf *buf) {
    AWS_PRECONDITION(aws_ring_buffer_is_valid(ring_buffer));
    AWS_PRECONDITION(aws_byte_buf_is_valid(buf));
    AWS_PRECONDITION(s_buf_belongs_to_pool(ring_buffer, buf));
    AWS_ATOMIC_STORE_TAIL_PTR(ring_buffer, buf->buffer + buf->capacity);
    AWS_ZERO_STRUCT(*buf);
    AWS_POSTCONDITION(aws_ring_buffer_is_valid(ring_buffer));
}