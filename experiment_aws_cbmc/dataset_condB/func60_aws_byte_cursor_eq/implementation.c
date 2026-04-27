aws_byte_cursor_eq(const struct aws_byte_cursor *a, const struct aws_byte_cursor *b) {
    bool rv = aws_array_eq(a->ptr, a->len, b->ptr, b->len);
    return rv;
}