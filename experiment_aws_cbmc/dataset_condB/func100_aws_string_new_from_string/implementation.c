aws_string_new_from_string(struct aws_allocator *allocator, const struct aws_string *str) {
    return aws_string_new_from_array(allocator, str->bytes, str->len);
}