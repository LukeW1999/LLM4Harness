aws_byte_buf_clean_up_secure(struct aws_byte_buf *buf) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(buf));
    aws_byte_buf_secure_zero(buf);
    aws_byte_buf_clean_up(buf);
    AWS_POSTCONDITION(aws_byte_buf_is_valid(buf));
}