aws_byte_buf_clean_up_secure(struct aws_byte_buf *buf) {
    aws_byte_buf_secure_zero(buf);
    aws_byte_buf_clean_up(buf);
}