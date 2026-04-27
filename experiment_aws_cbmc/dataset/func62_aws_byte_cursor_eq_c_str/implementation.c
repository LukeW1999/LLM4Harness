aws_byte_cursor_eq_c_str(const struct aws_byte_cursor *const cursor, const char *const c_str) {
    AWS_PRECONDITION(aws_byte_cursor_is_valid(cursor));
    AWS_PRECONDITION(c_str != NULL);
    bool rv = aws_array_eq_c_str(cursor->ptr, cursor->len, c_str);
    AWS_POSTCONDITION(aws_byte_cursor_is_valid(cursor));
    return rv;
}