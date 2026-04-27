aws_byte_cursor_read_be64(struct aws_byte_cursor *cur, uint64_t *var) {
    AWS_PRECONDITION(aws_byte_cursor_is_valid(cur));
    AWS_PRECONDITION(AWS_OBJECT_PTR_IS_WRITABLE(var));
    bool rv = aws_byte_cursor_read(cur, var, sizeof(*var));

    if (AWS_LIKELY(rv)) {
        *var = aws_ntoh64(*var);
    }

    AWS_POSTCONDITION(aws_byte_cursor_is_valid(cur));
    return rv;
}