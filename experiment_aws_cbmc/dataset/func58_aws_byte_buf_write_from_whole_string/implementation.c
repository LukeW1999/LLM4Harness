aws_byte_buf_write_from_whole_string(
    struct aws_byte_buf *AWS_RESTRICT buf,
    const struct aws_string *AWS_RESTRICT src) {
    AWS_PRECONDITION(!buf || aws_byte_buf_is_valid(buf));
    AWS_PRECONDITION(!src || aws_string_is_valid(src));
    if (buf == NULL || src == NULL) {
        return false;
    }
    return aws_byte_buf_write(buf, aws_string_bytes(src), src->len);
}