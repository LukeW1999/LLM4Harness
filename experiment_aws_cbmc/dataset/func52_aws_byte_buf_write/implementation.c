aws_byte_buf_write(struct aws_byte_buf *AWS_RESTRICT buf, const uint8_t *AWS_RESTRICT src, size_t len) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(buf));
    AWS_PRECONDITION(AWS_MEM_IS_READABLE(src, len), "Input array [src] must be readable up to [len] bytes.");

    if (len == 0) {
        AWS_POSTCONDITION(aws_byte_buf_is_valid(buf));
        return true;
    }

    if (buf->len > (SIZE_MAX >> 1) || len > (SIZE_MAX >> 1) || buf->len + len > buf->capacity) {
        AWS_POSTCONDITION(aws_byte_buf_is_valid(buf));
        return false;
    }

    memcpy(buf->buffer + buf->len, src, len);
    buf->len += len;

    AWS_POSTCONDITION(aws_byte_buf_is_valid(buf));
    return true;
}