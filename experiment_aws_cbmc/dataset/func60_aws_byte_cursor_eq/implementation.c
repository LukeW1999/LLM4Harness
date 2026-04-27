aws_byte_cursor_eq(const struct aws_byte_cursor *a, const struct aws_byte_cursor *b) {
    AWS_PRECONDITION(aws_byte_cursor_is_valid(a));
    AWS_PRECONDITION(aws_byte_cursor_is_valid(b));
    bool rv = aws_array_eq(a->ptr, a->len, b->ptr, b->len);
    AWS_POSTCONDITION(aws_byte_cursor_is_valid(a));
    AWS_POSTCONDITION(aws_byte_cursor_is_valid(b));
    return rv;
}