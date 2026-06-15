#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    assert(result == (a * b <= SIZE_MAX) ? a * b : SIZE_MAX);
}

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    int ret = aws_mul_size_checked(a, b, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result == a * b);
    } else {
        assert(ret == AWS_OP_ERR);
        assert(a * b > SIZE_MAX);
    }
}

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);

    assert(result == (a + b <= SIZE_MAX) ? a + b : SIZE_MAX);
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    int ret = aws_add_size_checked(a, b, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result == a + b);
    } else {
        assert(ret == AWS_OP_ERR);
        assert(a + b > SIZE_MAX);
    }
}

void aws_sub_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_sub_size_saturating(a, b);

    assert(result == (a >= b) ? a - b : 0);
}

void aws_sub_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    int ret = aws_sub_size_checked(a, b, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result == a - b);
    } else {
        assert(ret == AWS_OP_ERR);
        assert(a < b);
    }
}

void aws_is_power_of_two_harness() {
    size_t x = nondet_size_t();

    bool result = aws_is_power_of_two(x);

    assert(result == ((x != 0) && ((x & (x - 1)) == 0)));
}

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result >= n);
        assert((result & (result - 1)) == 0);
    } else {
        assert(ret == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
