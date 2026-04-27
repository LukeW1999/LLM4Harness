aws_string_eq_c_str(const struct aws_string *str, const char *c_str) {
    AWS_PRECONDITION(!str || aws_string_is_valid(str));
    if (str == NULL && c_str == NULL) {
        return true;
    }
    if (str == NULL || c_str == NULL) {
        return false;
    }
    return aws_array_eq_c_str(str->bytes, str->len, c_str);
}