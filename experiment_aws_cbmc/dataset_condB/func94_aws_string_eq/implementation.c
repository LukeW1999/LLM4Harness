aws_string_eq(const struct aws_string *a, const struct aws_string *b) {
    if (a == b) {
        return true;
    }
    if (a == NULL || b == NULL) {
        return false;
    }
    return aws_array_eq(a->bytes, a->len, b->bytes, b->len);
}