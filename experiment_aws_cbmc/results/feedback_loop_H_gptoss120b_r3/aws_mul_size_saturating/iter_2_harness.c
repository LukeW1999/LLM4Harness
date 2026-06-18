#include <aws/common/math.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    /* 1. Declare nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Allocate result on the stack and give it a nondeterministic initial value */
    size_t result = nondet_size_t();

    /* Save old state */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result;

    /* 2. Call function under test */
    int rc = aws_mul_size_saturating(a, b, &result);

    /* 3. Postcondition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* No overflow occurred: the product fits in size_t */
        if (a != 0) {
            assert(result / a == b);
        }
    } else {
        /* Overflow occurred: result must be saturated to SIZE_MAX */
        assert(result == SIZE_MAX);
    }

    /* 4. Unchanged inputs */
    assert(a == old_a);
    assert(b == old_b);
    assert(result == result); /* result may change, keep to silence unused warnings */
}
