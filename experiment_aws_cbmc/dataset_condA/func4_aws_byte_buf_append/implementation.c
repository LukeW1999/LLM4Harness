/* From: source/byte_buf.c */

int aws_byte_buf_append(struct aws_byte_buf *to, const struct aws_byte_cursor *from) {
    AWS_PRECONDITION(aws_byte_buf_is_valid(to));
    AWS_PRECONDITION(aws_byte_cursor_is_valid(from));

    if (to->capacity - to->len < from->len) {
        AWS_POSTCONDITION(aws_byte_buf_is_valid(to));
        AWS_POSTCONDITION(aws_byte_cursor_is_valid(from));
        return aws_raise_error(AWS_ERROR_DEST_COPY_TOO_SMALL);
    }

    if (from->len > 0) {
        AWS_ASSERT(from->ptr);
        AWS_ASSERT(to->buffer);
        memcpy(to->buffer + to->len, from->ptr, from->len);
        to->len += from->len;
    }

    AWS_POSTCONDITION(aws_byte_buf_is_valid(to));
    AWS_POSTCONDITION(aws_byte_cursor_is_valid(from));
    return AWS_OP_SUCCESS;
}
