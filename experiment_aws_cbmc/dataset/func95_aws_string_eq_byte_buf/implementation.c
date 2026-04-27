aws_string_eq_byte_buf(const struct aws_string *str, const struct aws_byte_buf *buf) {
    AWS_PRECONDITION(!str || aws_string_is_valid(str));
    AWS_PRECONDITION(!buf || aws_byte_buf_is_valid(buf));
    if (str == NULL && buf == NULL) {
        return true;
    }
    if (str == NULL || buf == NULL) {
        return false;
    }
    return aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
}