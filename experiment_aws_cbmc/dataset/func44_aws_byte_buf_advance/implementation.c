aws_byte_buf_advance(
    struct aws_byte_buf *const AWS_RESTRICT buffer,
    struct aws_byte_buf *const AWS_RESTRICT output,
    const size_t len) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(buffer));
    AWS_PRECONDITION(aws_byte_buf_is_valid(output));
    if (buffer->capacity - buffer->len >= len) {
        *output = aws_byte_buf_from_array((buffer->buffer == NULL) ? NULL : buffer->buffer + buffer->len, len);
        buffer->len += len;
        output->len = 0;
        AWS_POSTCONDITION(aws_byte_buf_is_valid(buffer));
        AWS_POSTCONDITION(aws_byte_buf_is_valid(output));
        return true;
    } else {
        AWS_ZERO_STRUCT(*output);
        AWS_POSTCONDITION(aws_byte_buf_is_valid(buffer));
        AWS_POSTCONDITION(aws_byte_buf_is_valid(output));
        return false;
    }
}