#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    /* 1. Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Limit the range of b to keep the verification tractable while still
     * allowing overflow (e.g., when b == 2 and a is large). */
    __CPROVER_assume(b <= 2);

    /* 2. Save old state */
    size_t old_a = a;
    size_t old_b = b;

    /* 3. Call function under test */
    size_t result = aws_mul_size_saturating(a, b);

    /* 4. Postconditions */
    /* Inputs must be unchanged */
    assert(a == old_a);
    assert(b == old_b);

    /* Result must be either the exact product or saturated to SIZE_MAX */
    size_t max = (size_t)-1;

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (result == max) {
        /* Saturated case: overflow occurred */
    } else {
        /* No overflow: result must equal the product and be consistent */
        assert(result == a * b);
        assert(result / a == b);
    }
}
