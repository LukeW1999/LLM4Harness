/* From: include/aws/common/math.fallback.inl and math.inl */

/* AWS_OP_SUCCESS = 0, AWS_OP_ERR = -1 */

AWS_STATIC_IMPL int aws_add_u64_checked(uint64_t a, uint64_t b, uint64_t *r) {
    if ((b > 0) && (a > (UINT64_MAX - b)))
        return aws_raise_error(AWS_ERROR_OVERFLOW_DETECTED);
    *r = a + b;
    return AWS_OP_SUCCESS;
}

AWS_STATIC_IMPL int aws_add_u32_checked(uint32_t a, uint32_t b, uint32_t *r) {
    if ((b > 0) && (a > (UINT32_MAX - b)))
        return aws_raise_error(AWS_ERROR_OVERFLOW_DETECTED);
    *r = a + b;
    return AWS_OP_SUCCESS;
}

AWS_STATIC_IMPL int aws_add_size_checked(size_t a, size_t b, size_t *r) {
#if SIZE_BITS == 32
    return aws_add_u32_checked(a, b, (uint32_t *)r);
#elif SIZE_BITS == 64
    return aws_add_u64_checked(a, b, (uint64_t *)r);
#endif
}
