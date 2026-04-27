aws_byte_buf_advance(
    struct aws_byte_buf *const AWS_RESTRICT buffer,
    struct aws_byte_buf *const AWS_RESTRICT output,
    const size_t len) {
    if (buffer->capacity - buffer->len >= len) {
        *output = aws_byte_buf_from_array((buffer->buffer == NULL) ? NULL : buffer->buffer + buffer->len, len);
        buffer->len += len;
        output->len = 0;
        return true;
    } else {
        AWS_ZERO_STRUCT(*output);
        return false;
    }
}