aws_array_eq_c_str(const void *const array, const size_t array_len, const char *const c_str) {
    AWS_PRECONDITION(
        array || (array_len == 0),
        "Either input pointer [array_a] mustn't be NULL or input [array_len] mustn't be zero.");

    /* Simpler implementation could have been:
     *   return aws_array_eq(array, array_len, c_str, strlen(c_str));
     * but that would have traversed c_str twice.
     * This implementation traverses c_str just once. */

    const uint8_t *array_bytes = array;
    const uint8_t *str_bytes = (const uint8_t *)c_str;

    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = str_bytes[i];
        if (s == '\0') {
            return false;
        }

        if (array_bytes[i] != s) {
            return false;
        }
    }

    return str_bytes[array_len] == '\0';
}