aws_byte_cursor_from_string(const struct aws_string *src) {
    if (!src) {
        struct aws_byte_cursor cursor;
        AWS_ZERO_STRUCT(cursor);
        return cursor;
    }

    return aws_byte_cursor_from_array(aws_string_bytes(src), src->len);
}