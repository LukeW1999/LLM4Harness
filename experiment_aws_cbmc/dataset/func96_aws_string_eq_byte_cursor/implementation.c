aws_string_eq_byte_cursor(const struct aws_string *str, const struct aws_byte_cursor *cur) {
    AWS_PRECONDITION(!str || aws_string_is_valid(str));
    AWS_PRECONDITION(!cur || aws_byte_cursor_is_valid(cur));
    if (str == NULL && cur == NULL) {
        return true;
    }
    if (str == NULL || cur == NULL) {
        return false;
    }
    return aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
}