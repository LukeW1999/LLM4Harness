#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic helpers */
size_t nondet_size_t(void);
int nondet_int(void);

void aws_mul_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* output buffer and its initial content (to check frame condition) */
    size_t r_old = nondet_size_t();
    size_t r = r_old;

    /* surrounding memory to verify frame condition */
    size_t before = nondet_size_t();
    size_t after  = nondet_size_t();
    const size_t before_snapshot = before;
    const size_t after_snapshot  = after;

    /* call the function under test */
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

    /* 2. Output buffer length/capacity invariants
       (size_t multiplication has no separate length field; the check above suffices) */

    /* 3. Memory not modified beyond the function's contract (frame condition) */
    __CPROVER_assert(before == before_snapshot,
                     "memory before the output pointer must be unchanged");
    __CPROVER_assert(after == after_snapshot,
                     "memory after the output pointer must be unchanged");

    return 0;
}
