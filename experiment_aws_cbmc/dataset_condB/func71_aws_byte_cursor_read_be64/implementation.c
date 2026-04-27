aws_byte_cursor_read_be64(struct aws_byte_cursor *cur, uint64_t *var) {
    bool rv = aws_byte_cursor_read(cur, var, sizeof(*var));

    if (AWS_LIKELY(rv)) {
        *var = aws_ntoh64(*var);
    }

    return rv;
}