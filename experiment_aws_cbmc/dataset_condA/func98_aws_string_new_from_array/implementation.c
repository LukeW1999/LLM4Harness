aws_string_new_from_array(struct aws_allocator *allocator, const uint8_t *bytes, size_t len) {

    struct aws_string *str = aws_mem_acquire(allocator, offsetof(struct aws_string, bytes[len + 1]));
    if (!str) {
        return NULL;
    }

    /* Fields are declared const, so we need to copy them in like this */
    *(struct aws_allocator **)(&str->allocator) = allocator;
    *(size_t *)(&str->len) = len;
    if (len > 0) {
        memcpy((void *)str->bytes, bytes, len);
    }
    *(uint8_t *)&str->bytes[len] = 0;
    AWS_RETURN_WITH_POSTCONDITION(str, aws_string_is_valid(str));
}