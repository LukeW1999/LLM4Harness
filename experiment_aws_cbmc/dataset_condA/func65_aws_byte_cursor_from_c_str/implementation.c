aws_byte_cursor_from_c_str(const char *c_str) {
    struct aws_byte_cursor cur;
    cur.ptr = (uint8_t *)c_str;
    cur.len = (cur.ptr) ? strlen(c_str) : 0;
    return cur;
}