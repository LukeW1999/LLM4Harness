#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate result storage and give it a nondeterministic initial value */
    size_t result = nondet_size_t();

    /* save old state */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result;

    /* call the function under test */
    int rc = aws_mul_size_saturating(a, b, &result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be the exact product of a and b */
        if (a != 0) {
            assert(result / a == b);
        }
        if (b != 0) {
            assert(result / b == a);
        }
    } else {
        /* on overflow, result must be unchanged */
        assert(result == old_result);
    }

    /* inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);
}
