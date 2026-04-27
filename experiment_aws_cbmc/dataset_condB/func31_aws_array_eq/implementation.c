aws_array_eq(const void *const array_a, const size_t len_a, const void *const array_b, const size_t len_b) {
    AWS_PRECONDITION(
        (len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a), "Input array [array_a] must be readable up to [len_a].");
    AWS_PRECONDITION(
        (len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b), "Input array [array_b] must be readable up to [len_b].");

    if (len_a != len_b) {
        return false;
    }

    if (len_a == 0) {
        return true;
    }

    return !memcmp(array_a, array_b, len_a);
}