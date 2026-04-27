aws_byte_cursor_read_u8(struct aws_byte_cursor *AWS_RESTRICT cur, uint8_t *AWS_RESTRICT var) {
    bool rv = aws_byte_cursor_read(cur, var, 1);
    return rv;
}