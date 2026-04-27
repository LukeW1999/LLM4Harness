aws_byte_buf_append_and_update(struct aws_byte_buf *to, struct aws_byte_cursor *from_and_update) {

    if (aws_byte_buf_append(to, from_and_update)) {
        return AWS_OP_ERR;
    }

    from_and_update->ptr = to->buffer == NULL ? NULL : to->buffer + (to->len - from_and_update->len);
    return AWS_OP_SUCCESS;
}