/* mock iter 3 harness — sacrifice-then-recover: re-adds the deleted assertion */
void harness(void) {
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buf != NULL);
    size_t capacity;
    __CPROVER_assume(capacity <= 1024);

    int result = aws_byte_buf_init(buf, allocator, capacity);

    if (result == 0) {
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        assert(buf->allocator == allocator);           /* RE-ADDED (was deleted in iter 2) */
        assert(buf->buffer != NULL || capacity == 0);
        assert(buf->len <= buf->capacity);
    } else {
        assert(buf->len == 0);
        assert(buf->capacity <= 1);
        assert(buf->allocator == NULL);
    }
}
