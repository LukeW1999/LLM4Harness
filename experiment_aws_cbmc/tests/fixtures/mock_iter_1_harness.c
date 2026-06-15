/* mock iter 1 harness — baseline with multiple assertions */
void harness(void) {
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buf != NULL);
    size_t capacity;
    __CPROVER_assume(capacity <= 1024);

    int result = aws_byte_buf_init(buf, allocator, capacity);

    if (result == 0) {
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        assert(buf->allocator == allocator);
        assert(buf->buffer != NULL || capacity == 0);
    } else {
        assert(buf->len == 0);
        assert(buf->capacity == 0);
        assert(buf->allocator == NULL);
    }
}
