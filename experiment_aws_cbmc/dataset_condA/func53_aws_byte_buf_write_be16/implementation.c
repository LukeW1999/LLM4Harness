aws_byte_buf_write_be16(struct aws_byte_buf *buf, uint16_t x) {
    x = aws_hton16(x);
    return aws_byte_buf_write(buf, (uint8_t *)&x, 2);
}