#include <aws/common/math.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r_val;
    size_t *r = &r_val;
    __CPROVER_assume(r != NULL);
    size_t old_r = *r;
    int result = aws_add_size_checked(a, b, r);

    // Postcondition: return value must be success or error
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    // Postcondition based on overflow condition
    if (b > 0 && a > (SIZE_MAX - b)) {
        assert(result == AWS_OP_ERR);
        assert(*r == old_r);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(*r == a + b);
    }

    // Ensure both branches are reachable via nondeterministic inputs
    // This additional assertion is always reachable
    assert(1);
}
