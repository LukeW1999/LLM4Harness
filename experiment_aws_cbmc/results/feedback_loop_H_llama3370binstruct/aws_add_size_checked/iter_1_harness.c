#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    __CPROVER_assume(a <= UINT64_MAX);
    __CPROVER_assume(b <= UINT64_MAX);

    int result = aws_add_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a > (UINT64_MAX - b));
    }

    assert(true); // No other fields to check
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    __CPROVER_assume(a <= UINT32_MAX);
    __CPROVER_assume(b <= UINT32_MAX);

    int result = aws_add_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a > (UINT32_MAX - b));
    }

    assert(true); // No other fields to check
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a > (SIZE_MAX - b));
    }

    assert(true); // No other fields to check
}
