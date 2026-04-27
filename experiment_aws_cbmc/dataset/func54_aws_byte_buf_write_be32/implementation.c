aws_byte_buf_write_be32(struct aws_byte_buf *buf, uint32_t x) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(buf));
    x = aws_hton32(x);
    return aws_byte_buf_write(buf, (uint8_t *)&x, 4);
}