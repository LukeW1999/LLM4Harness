aws_byte_buf_write(struct aws_byte_buf *AWS_RESTRICT buf, const uint8_t *AWS_RESTRICT src, size_t len) {

    if (len == 0) {
        return true;
    }

    if (buf->len > (SIZE_MAX >> 1) || len > (SIZE_MAX >> 1) || buf->len + len > buf->capacity) {
        return false;
    }

    memcpy(buf->buffer + buf->len, src, len);
    buf->len += len;

    return true;
}