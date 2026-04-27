#include <stdint.h>
#include <stddef.h>
#include "proof_helpers/make_common_data_structures.h"

#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR -1

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
    uint64_t *r_ptr = &r;

    /* 1. Declare and bound data structures */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r_ptr, sizeof(uint64_t)));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    uint64_t old_r = r;

    /* 3. Call function under test */
    int result = aws_add_u64_checked(a, b, r_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to check for this function */

    /* 6. Assert validity invariants always holds */
    /* No additional validity invariants for this function */
}

AWS_STATIC_IMPL int aws_add_u32_checked(uint32_t a, uint32_t b, uint32_t *r) {
    if ((b > 0) && (a > (UINT32_MAX - b)))
        return aws_raise_error(AWS_ERROR_OVERFLOW_DETECTED);
    *r = a + b;
    return AWS_OP_SUCCESS;
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;
    uint32_t *r_ptr = &r;

    /* 1. Declare and bound data structures */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r_ptr, sizeof(uint32_t)));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    uint32_t old_r = r;

    /* 3. Call function under test */
    int result = aws_add_u32_checked(a, b, r_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to check for this function */

    /* 6. Assert validity invariants always holds */
    /* No additional validity invariants for this function */
}

AWS_STATIC_IMPL int aws_add_size_checked(size_t a, size_t b, size_t *r) {
#if SIZE_BITS == 32
    return aws_add_u32_checked(a, b, (uint32_t *)r);
#elif SIZE_BITS == 64
    return aws_add_u64_checked(a, b, (uint64_t *)r);
#endif
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* 1. Declare and bound data structures */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r_ptr, sizeof(size_t)));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_r = r;

    /* 3. Call function under test */
    int result = aws_add_size_checked(a, b, r_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to check for this function */

    /* 6. Assert validity invariants always holds */
    /* No additional validity invariants for this function */
}
