aws_byte_buf_init_copy(struct aws_byte_buf *dest, struct aws_allocator *allocator, const struct aws_byte_buf *src) {
    AWS_PRECONDITION(allocator);
    AWS_PRECONDITION(dest);
    AWS_ERROR_PRECONDITION(aws_byte_buf_is_valid(src));

    if (!src->buffer) {
        AWS_ZERO_STRUCT(*dest);
        dest->allocator = allocator;
        AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
        return AWS_OP_SUCCESS;
    }

    *dest = *src;
    dest->allocator = allocator;
    dest->buffer = (uint8_t *)aws_mem_acquire(allocator, src->capacity);
    if (dest->buffer == NULL) {
        AWS_ZERO_STRUCT(*dest);
        return AWS_OP_ERR;
    }
    memcpy(dest->buffer, src->buffer, src->len);
    AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
    return AWS_OP_SUCCESS;
}