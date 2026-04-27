aws_byte_cursor_from_buf(const struct aws_byte_buf *const buf) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(buf));
    struct aws_byte_cursor cur;
    cur.ptr = buf->buffer;
    cur.len = buf->len;
    AWS_POSTCONDITION(aws_byte_cursor_is_valid(&cur));
    return cur;
}