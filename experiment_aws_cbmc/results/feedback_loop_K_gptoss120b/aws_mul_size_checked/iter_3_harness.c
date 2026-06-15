#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_mul_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    /* limit the range to keep verification tractable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    /* nondeterministic initial value for the result location */
    size_t r = (size_t)nondet_uint64_t();
    size_t r_old = r;

    /* call the function under test */
    int ret = aws_mul_size_checked(a, b, &r);

    /* postconditions */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* no overflow must have occurred */
        assert(a == 0 || b <= SIZE_MAX / a);
        /* result must be the exact product */
        assert(r == a * b);
    } else {
        /* overflow must have occurred */
        assert(a != 0 && b > SIZE_MAX / a);
        /* result must be unchanged on error */
        assert(r == r_old);
    }
}
