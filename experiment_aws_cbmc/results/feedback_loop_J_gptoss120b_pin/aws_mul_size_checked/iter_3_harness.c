#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* nondeterministically initialize the result */
    size_t result = nondet_size_t();

    /* save old state */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result;

    /* call function under test */
    int rc = aws_mul_size_checked(a, b, &result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be the product of a and b */
        assert(result == old_a * old_b);
        /* overflow must not have occurred */
        if (old_a != 0) {
            assert(result / old_a == old_b);
        }
        /* inputs must be unchanged */
        assert(a == old_a);
        assert(b == old_b);
    } else {
        /* on failure, result must be unchanged */
        assert(result == old_result);
        /* inputs must be unchanged */
        assert(a == old_a);
        assert(b == old_b);
    }
}
