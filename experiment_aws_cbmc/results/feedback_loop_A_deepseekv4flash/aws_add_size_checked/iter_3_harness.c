#include <aws/common/math.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_size_checked_harness() {
    size_t a, b, r;
    int result;

    // Overflow branch
    a = nondet_size_t();
    b = nondet_size_t();
    __CPROVER_assume(b > 0 && a > (SIZE_MAX - b));
    result = aws_add_size_checked(a, b, &r);
    assert(result == AWS_OP_ERR);

    // No-overflow branch
    a = nondet_size_t();
    b = nondet_size_t();
    __CPROVER_assume(a <= SIZE_MAX - b);
    result = aws_add_size_checked(a, b, &r);
    assert(result == AWS_OP_SUCCESS);
    assert(r == a + b);

    // Trivial assertion to guarantee reachable assertions
    assert(1);
}
