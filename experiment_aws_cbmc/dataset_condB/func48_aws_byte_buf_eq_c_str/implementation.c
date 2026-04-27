aws_byte_buf_eq_c_str(const struct aws_byte_buf *const buf, const char *const c_str) {
    bool rval = aws_array_eq_c_str(buf->buffer, buf->len, c_str);
    return rval;
}