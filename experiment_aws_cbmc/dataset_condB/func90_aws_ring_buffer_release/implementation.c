aws_ring_buffer_release(struct aws_ring_buffer *ring_buffer, struct aws_byte_buf *buf) {
    AWS_ATOMIC_STORE_TAIL_PTR(ring_buffer, buf->buffer + buf->capacity);
    AWS_ZERO_STRUCT(*buf);
}