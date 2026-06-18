#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = 0;
    size_t b = 0;
    size_t r = nondet_size_t();
    bool expect_overflow = false;

    size_t scenario = nondet_size_t();
    __CPROVER_assume(scenario < 8);

    if (scenario == 0) {
        a = 0;
        b = (size_t)-1;
    } else if (scenario == 1) {
        a = (size_t)-1;
        b = 0;
    } else if (scenario == 2) {
        a = (size_t)-1;
        b = 1;
    } else if (scenario == 3) {
        a = 1;
        b = (size_t)-1;
    } else if (scenario == 4) {
        a = 123;
        b = 456;
    } else if (scenario == 5) {
        a = (size_t)-1;
        b = 2;
        expect_overflow = true;
    } else if (scenario == 6) {
        a = 2;
        b = (size_t)-1;
        expect_overflow = true;
    } else {
        a = ((size_t)-1 / 2) + 1;
        b = 2;
        expect_overflow = true;
    }

    int result = aws_mul_size_checked(a, b, &r);

    if (expect_overflow) {
        __CPROVER_assert(result == AWS_OP_ERR, "overflow is reported");
    } else {
        __CPROVER_assert(result == AWS_OP_SUCCESS, "non-overflow succeeds");
        __CPROVER_assert(r == a * b, "result is the mathematical product");
    }
}
