aws_string_new_from_c_str(struct aws_allocator *allocator, const char *c_str) {
    return aws_string_new_from_array(allocator, (const uint8_t *)c_str, strlen(c_str));
}