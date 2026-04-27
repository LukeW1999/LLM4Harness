aws_byte_buf_write_from_whole_cursor(struct aws_byte_buf *AWS_RESTRICT buf, struct aws_byte_cursor src) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(buf));
    AWS_PRECONDITION(aws_byte_cursor_is_valid(&src));
    return aws_byte_buf_write(buf, src.ptr, src.len);
}