#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <cbmc/model_assert.h>

#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR -1
#define AWS_ERROR_OVERFLOW_DETECTED 1

int aws_raise_error(int error_code) {
    return error_code;
}

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

void aws_add_size_checked_harness() {
    /* 1. Declare data structures */
    size_t a;
    size_t b;
    size_t r;
    size_t *r_ptr = &r;

    /* 2. Bound and allocate structures */
    /* No additional allocation needed for primitive types */

    /* 3. Assume preconditions */
    __CPROVER_assume(r_ptr != NULL);

    /* 4. Save old state if needed */
    /* Not needed for this function */

    /* 5. Call function under verification */
    int result = aws_add_size_checked(a, b, r_ptr);

    /* 6. Assert postconditions (both success and failure branches) */
    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(a + b == r, "Addition should be correct on success");
    } else if (result == AWS_OP_ERR) {
        __CPROVER_assert(a + b < a || a + b < b, "Overflow detected correctly");
    }

    /* 7. Assert invariants (validity predicates) */
    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR, "Result should be either success or error");
}
