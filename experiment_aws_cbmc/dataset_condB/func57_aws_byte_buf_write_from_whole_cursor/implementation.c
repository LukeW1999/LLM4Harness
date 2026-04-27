aws_byte_buf_write_from_whole_cursor(struct aws_byte_buf *AWS_RESTRICT buf, struct aws_byte_cursor src) {
    return aws_byte_buf_write(buf, src.ptr, src.len);
}