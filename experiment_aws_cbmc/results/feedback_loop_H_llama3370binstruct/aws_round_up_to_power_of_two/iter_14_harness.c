#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;
    int res;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    res = aws_mul_size_checked(a, b, &result);

    if (res == AWS_OP_SUCCESS) {
        assert(result == a * b);
    } else {
        assert(a * b > SIZE_MAX);
        assert(result == 0); // result is not modified on failure
    }
}

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    result = aws_mul_size_saturating(a, b);

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
    int res;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    res = aws_add_size_checked(a, b, &result);

    if (res == AWS_OP_SUCCESS) {
        assert(result == a + b);
    } else {
        assert(a + b > SIZE_MAX);
        assert(result == 0); // result is not modified on failure
    }
}

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    result = aws_add_size_saturating(a, b);

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
    int res;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    res = aws_sub_size_checked(a, b, &result);

    if (res == AWS_OP_SUCCESS) {
        assert(result == a - b);
    } else {
        assert(a < b);
        assert(result == 0); // result is not modified on failure
    }
}

void aws_sub_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    result = aws_sub_size_saturating(a, b);

    if (a >= b) {
        assert(result == a - b);
    } else {
        assert(result == 0);
    }
}

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    int res;

    __CPROVER_assume(n <= SIZE_MAX);

    res = aws_round_up_to_power_of_two(n, &result);

    if (res == AWS_OP_SUCCESS) {
        assert(result >= n);
        assert((result & (result - 1)) == 0);
        assert(result <= SIZE_MAX);
    } else {
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == 0); // result is not modified on failure
    }
    if (n == 0) {
        assert(result == 1);
    } else if (n > SIZE_MAX_POWER_OF_TWO) {
        assert(result == 0);
    } else {
        assert(result >= n);
        assert((result & (result - 1)) == 0);
    }
    if (n > 0 && n <= SIZE_MAX_POWER_OF_TWO) {
        assert(result == n || (result > n && (result & (result - 1)) == 0));
    }
    if (n == SIZE_MAX) {
        assert(res == AWS_OP_ERR);
    }
    if (n > SIZE_MAX_POWER_OF_TWO) {
        assert(res == AWS_OP_ERR);
    }
    if (n == SIZE_MAX) {
        assert(result == 0);
    }
}

void aws_is_power_of_two_harness() {
    size_t x = nondet_size_t();
    bool result;

    __CPROVER_assume(x <= SIZE_MAX);

    result = aws_is_power_of_two(x);

    if (result) {
        assert((x & (x - 1)) == 0);
        assert(x != 0);
    } else {
        assert((x & (x - 1)) != 0 || x == 0);
    }
}
