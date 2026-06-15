#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result = aws_mul_size_saturating(a, b);

    assert(result == (a * b) || result == SIZE_MAX);
}

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a * b > SIZE_MAX);
    }
}

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result = aws_add_size_saturating(a, b);

    assert(result == (a + b) || result == SIZE_MAX);
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
        assert(a + b > SIZE_MAX);
    }
}

void aws_sub_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result = aws_sub_size_saturating(a, b);

    assert(result == (a - b) || result == 0);
}

void aws_sub_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    int result = aws_sub_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a < b);
    }
}

void aws_is_power_of_two_harness() {
    size_t x = nondet_size_t();

    __CPROVER_assume(x <= SIZE_MAX);

    bool result = aws_is_power_of_two(x);

    assert(result == ((x != 0) && ((x & (x - 1)) == 0)));
}

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;

    __CPROVER_assume(n <= SIZE_MAX);

    int res = aws_round_up_to_power_of_two(n, &result);

    if (res == AWS_OP_SUCCESS) {
        assert(result >= n);
        assert((result & (result - 1)) == 0);
    } else {
        assert(res == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
