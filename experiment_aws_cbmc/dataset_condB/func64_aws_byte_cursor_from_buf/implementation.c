aws_byte_cursor_from_buf(const struct aws_byte_buf *const buf) {
    struct aws_byte_cursor cur;
    cur.ptr = buf->buffer;
    cur.len = buf->len;
    return cur;
}