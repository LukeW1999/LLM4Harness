aws_byte_cursor_read_be32(struct aws_byte_cursor *cur, uint32_t *var) {
    bool rv = aws_byte_cursor_read(cur, var, 4);

    if (AWS_LIKELY(rv)) {
        *var = aws_ntoh32(*var);
    }

    return rv;
}