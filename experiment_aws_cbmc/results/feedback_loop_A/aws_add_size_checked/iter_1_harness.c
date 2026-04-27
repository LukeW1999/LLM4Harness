#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <limits.h>

#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR -1

// Mock implementation of aws_raise_error for completeness
int aws_raise_error(int error_code) {
    return AWS_OP_ERR;
}

AWS_STATIC_IMPL int aws_add_u64_checked(uint64_t a, uint64_t b, uint64_t *r) {
    if ((b > 0) && (a > (UINT64_MAX - b)))
        return aws_raise_error(AWS_ERROR_OVERFLOW_DETECTED);
    *r = a + b;
    return AWS_OP_SUCCESS;
}

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t old_r = r;

    // Assume r is writable
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));

    int result = aws_add_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r); // r should not change on failure
    }

    // No other fields to check for this function, but ensure no overflow occurred if success
    if (result == AWS_OP_SUCCESS) {
        assert(!(b > 0 && a > (UINT64_MAX - b)));
    } else {
        assert(b > 0 && a > (UINT64_MAX - b));
    }
}
