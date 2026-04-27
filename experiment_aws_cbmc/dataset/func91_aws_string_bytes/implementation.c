aws_string_bytes(const struct aws_string *str) {
    AWS_PRECONDITION(aws_string_is_valid(str));
    return str->bytes;
}