aws_byte_cursor_read_u8(struct aws_byte_cursor *AWS_RESTRICT cur, uint8_t *AWS_RESTRICT var) {
    AWS_PRECONDITION(aws_byte_cursor_is_valid(cur));
    AWS_PRECONDITION(AWS_MEM_IS_WRITABLE(var, 1));
    bool rv = aws_byte_cursor_read(cur, var, 1);
    AWS_POSTCONDITION(aws_byte_cursor_is_valid(cur));
    return rv;
}