aws_byte_buf_from_c_str(const char *c_str) {
    struct aws_byte_buf buf;
    buf.len = (!c_str) ? 0 : strlen(c_str);
    buf.capacity = buf.len;
    buf.buffer = (buf.capacity == 0) ? NULL : (uint8_t *)c_str;
    buf.allocator = NULL;
    AWS_POSTCONDITION(aws_byte_buf_is_valid(&buf));
    return buf;
}