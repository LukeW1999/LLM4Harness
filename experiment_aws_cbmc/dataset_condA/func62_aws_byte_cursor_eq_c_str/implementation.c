aws_byte_cursor_eq_c_str(const struct aws_byte_cursor *const cursor, const char *const c_str) {
    bool rv = aws_array_eq_c_str(cursor->ptr, cursor->len, c_str);
    return rv;
}