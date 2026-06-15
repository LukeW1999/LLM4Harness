#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    int ret = aws_mul_size_checked(a, b, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result == a * b);
    } else {
        assert(a * b > SIZE_MAX);
        assert(result == 0); // result is not modified on failure
    }

    // No fields to check for immutability
}

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result = aws_mul_size_saturating(a, b);

    if (a * b <= SIZE_MAX) {
        assert(result == a * b);
    } else {
        assert(result == SIZE_MAX);
    }
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    int ret = aws_add_size_checked(a, b, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result == a + b);
    } else {
        assert(a + b > SIZE_MAX);
        assert(result == 0); // result is not modified on failure
    }

    // No fields to check for immutability
}

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result = aws_add_size_saturating(a, b);

    if (a + b <= SIZE_MAX) {
        assert(result == a + b);
    } else {
        assert(result == SIZE_MAX);
    }
}

void aws_sub_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    int ret = aws_sub_size_checked(a, b, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result == a - b);
    } else {
        assert(a < b);
        assert(result == 0); // result is not modified on failure
    }

    // No fields to check for immutability
}

void aws_sub_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result = aws_sub_size_saturating(a, b);

    if (a >= b) {
        assert(result == a - b);
    } else {
        assert(result == 0);
    }
}

void aws_is_power_of_two_harness() {
    size_t x = nondet_size_t();

    __CPROVER_assume(x <= SIZE_MAX);

    bool result = aws_is_power_of_two(x);

    if (x == 0) {
        assert(!result);
    } else if ((x & (x - 1)) == 0) {
        assert(result);
    } else {
        assert(!result);
    }
}

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;

    __CPROVER_assume(n <= SIZE_MAX);

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result >= n);
        assert((result & (result - 1)) == 0);
    } else {
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == 0); // result is not modified on failure
    }

    // No fields to check for immutability
}
