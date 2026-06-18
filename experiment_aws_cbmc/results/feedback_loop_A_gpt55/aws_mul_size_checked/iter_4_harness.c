#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= (size_t)255);
    __CPROVER_assume(b <= (size_t)255);

    size_t result = nondet_size_t();
    int rv = aws_mul_size_checked(a, b, &result);

    unsigned int expected = (unsigned int)a * (unsigned int)b;

    __CPROVER_assert(rv == AWS_OP_SUCCESS, "aws_mul_size_checked succeeds without overflow");
    __CPROVER_assert(result == (size_t)expected, "aws_mul_size_checked returns the product");

    size_t overflow_result = nondet_size_t();
    size_t old_overflow_result = overflow_result;

    rv = aws_mul_size_checked((size_t)-1, (size_t)2, &overflow_result);

    __CPROVER_assert(rv == AWS_OP_ERR, "aws_mul_size_checked fails on overflow");
    __CPROVER_assert(overflow_result == old_overflow_result, "aws_mul_size_checked does not modify result on overflow");
}
