aws_byte_cursor_read_be16(struct aws_byte_cursor *cur, uint16_t *var) {
    bool rv = aws_byte_cursor_read(cur, var, 2);

    if (AWS_LIKELY(rv)) {
        *var = aws_ntoh16(*var);
    }

    return rv;
}