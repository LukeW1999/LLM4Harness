aws_byte_buf_write_from_whole_buffer(struct aws_byte_buf *AWS_RESTRICT buf, struct aws_byte_buf src) {
    return aws_byte_buf_write(buf, src.buffer, src.len);
}