/* From: source/byte_buf.c */

int aws_byte_buf_init(struct aws_byte_buf *buf, struct aws_allocator *allocator, size_t capacity) {
    AWS_PRECONDITION(buf);
    AWS_PRECONDITION(allocator);

    buf->buffer = (capacity == 0) ? NULL : aws_mem_acquire(allocator, capacity);
    if (capacity != 0 && buf->buffer == NULL) {
        AWS_ZERO_STRUCT(*buf);
        return AWS_OP_ERR;
    }

    buf->len = 0;
    buf->capacity = capacity;
    buf->allocator = allocator;
    AWS_POSTCONDITION(aws_byte_buf_is_valid(buf));
    return AWS_OP_SUCCESS;
}
