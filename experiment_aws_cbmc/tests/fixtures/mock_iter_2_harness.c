/* mock iter 2 harness — one assertion deleted, one weakened, one added */
void harness(void) {
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buf != NULL);
    size_t capacity;
    __CPROVER_assume(capacity <= 1024);

    int result = aws_byte_buf_init(buf, allocator, capacity);

    if (result == 0) {
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        /* DELETED: assert(buf->allocator == allocator); */
        assert(buf->buffer != NULL || capacity == 0);  /* unchanged */
        assert(buf->len <= buf->capacity);             /* new assertion added */
    } else {
        assert(buf->len == 0);
        /* WEAKENED: assert(buf->capacity == 0) -> assert(buf->capacity <= 1) */
        assert(buf->capacity <= 1);
        assert(buf->allocator == NULL);
    }
}
