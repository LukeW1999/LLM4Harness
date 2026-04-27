aws_byte_cursor_eq_byte_buf(const struct aws_byte_cursor *const a, const struct aws_byte_buf *const b) {
    bool rv = aws_array_eq(a->ptr, a->len, b->buffer, b->len);
    return rv;
}