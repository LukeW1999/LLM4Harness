#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdbool.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic size_t generator provided by CBMC */
size_t nondet_size_t(void);

void aws_mul_size_checked_harness(void) {
    /* Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Output buffer and its initial content (to check frame condition) */
    size_t r_old = nondet_size_t();
    size_t r = r_old;

    /* Call the function under test */
    int ret = aws_mul_size_checked(a, b, &r);

    /* 1. Return value / error code correctness */
    __CPROVER_assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR,
                     "aws_mul_size_checked must return AWS_OP_SUCCESS or AWS_OP_ERR");

    /* Determine whether overflow would occur */
    bool overflow = (a != 0 && b > SIZE_MAX / a);

    if (ret == AWS_OP_SUCCESS) {
        __CPROVER_assert(!overflow,
                         "aws_mul_size_checked returned success only when no overflow occurs");
        __CPROVER_assert(r == a * b,
                         "result must equal a * b on success");
    } else {
        __CPROVER_assert(overflow,
                         "aws_mul_size_checked returned error only when overflow occurs");
        __CPROVER_assert(r == r_old,
                         "output buffer must remain unchanged on error");
    }
}
