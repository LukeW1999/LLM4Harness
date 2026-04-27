aws_string_destroy(struct aws_string *str) {
    if (str && str->allocator) {
        aws_mem_release(str->allocator, str);
    }
}