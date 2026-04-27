aws_byte_buf_cat(struct aws_byte_buf *dest, size_t number_of_args, ...) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(dest));

    va_list ap;
    va_start(ap, number_of_args);

    for (size_t i = 0; i < number_of_args; ++i) {
        struct aws_byte_buf *buffer = va_arg(ap, struct aws_byte_buf *);
        struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(buffer);

        if (aws_byte_buf_append(dest, &cursor)) {
            va_end(ap);
            AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
            return AWS_OP_ERR;
        }
    }

    va_end(ap);
    AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
    return AWS_OP_SUCCESS;
}