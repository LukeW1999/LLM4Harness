aws_byte_buf_write_from_whole_buffer(struct aws_byte_buf *AWS_RESTRICT buf, struct aws_byte_buf src) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(buf));
    AWS_PRECONDITION(aws_byte_buf_is_valid(&src));
    return aws_byte_buf_write(buf, src.buffer, src.len);
}