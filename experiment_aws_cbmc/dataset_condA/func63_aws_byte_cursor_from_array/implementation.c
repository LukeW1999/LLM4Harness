aws_byte_cursor_from_array(const void *const bytes, const size_t len) {
    struct aws_byte_cursor cur;
    cur.ptr = (uint8_t *)bytes;
    cur.len = len;
    return cur;
}