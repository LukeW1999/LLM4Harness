aws_byte_cursor_from_array(const void *const bytes, const size_t len) {
    AWS_PRECONDITION(len == 0 || AWS_MEM_IS_READABLE(bytes, len), "Input array [bytes] must be readable up to [len].");
    struct aws_byte_cursor cur;
    cur.ptr = (uint8_t *)bytes;
    cur.len = len;
    AWS_POSTCONDITION(aws_byte_cursor_is_valid(&cur));
    return cur;
}