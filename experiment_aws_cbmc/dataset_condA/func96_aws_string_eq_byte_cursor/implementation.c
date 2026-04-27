aws_string_eq_byte_cursor(const struct aws_string *str, const struct aws_byte_cursor *cur) {
    if (str == NULL && cur == NULL) {
        return true;
    }
    if (str == NULL || cur == NULL) {
        return false;
    }
    return aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
}