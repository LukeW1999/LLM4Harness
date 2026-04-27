aws_byte_buf_init_copy_from_cursor(
    struct aws_byte_buf *dest,
    struct aws_allocator *allocator,
    struct aws_byte_cursor src) {
    AWS_ERROR_PRECONDITION(aws_byte_cursor_is_valid(&src));

    AWS_ZERO_STRUCT(*dest);

    dest->buffer = (src.len > 0) ? (uint8_t *)aws_mem_acquire(allocator, src.len) : NULL;
    if (src.len != 0 && dest->buffer == NULL) {
        return AWS_OP_ERR;
    }

    dest->len = src.len;
    dest->capacity = src.len;
    dest->allocator = allocator;
    if (src.len > 0) {
        memcpy(dest->buffer, src.ptr, src.len);
    }
    return AWS_OP_SUCCESS;
}