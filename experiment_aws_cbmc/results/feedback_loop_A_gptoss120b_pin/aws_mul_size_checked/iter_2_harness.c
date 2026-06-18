#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* bound the inputs to keep the state space reasonable */
    __CPROVER_assume(a <= 1024);
    __CPROVER_assume(b <= 1024);

    /* result buffer */
    size_t result = nondet_size_t();
    size_t old_result = result;

    /* call the function under test */
    int ret = aws_mul_size_checked(a, b, &result);

    /* post‑conditions */
    if (ret == 0) {
        /* no overflow: result must be the exact product */
        __CPROVER_assert(result == a * b, "result equals a * b on success");
    } else {
        /* overflow: result must be left unchanged */
        __CPROVER_assert(result == old_result, "result unchanged on overflow");
    }
}
