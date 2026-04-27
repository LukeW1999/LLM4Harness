aws_byte_cursor_eq_byte_buf(const struct aws_byte_cursor *const a, const struct aws_byte_buf *const b) {
    AWS_PRECONDITION(aws_byte_cursor_is_valid(a));
    AWS_PRECONDITION(aws_byte_buf_is_valid(b));
    bool rv = aws_array_eq(a->ptr, a->len, b->buffer, b->len);
    AWS_POSTCONDITION(aws_byte_cursor_is_valid(a));
    AWS_POSTCONDITION(aws_byte_buf_is_valid(b));
    return rv;
}