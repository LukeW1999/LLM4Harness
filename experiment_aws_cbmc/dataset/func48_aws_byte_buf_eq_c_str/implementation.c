aws_byte_buf_eq_c_str(const struct aws_byte_buf *const buf, const char *const c_str) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(buf));
    AWS_PRECONDITION(c_str != NULL);
    bool rval = aws_array_eq_c_str(buf->buffer, buf->len, c_str);
    AWS_POSTCONDITION(aws_byte_buf_is_valid(buf));
    return rval;
}