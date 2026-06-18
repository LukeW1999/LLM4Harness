#include <aws/common/math.h>
#include <assert.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    /* Symbolic inputs */
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    /* Snapshot of inputs for postcondition checks */
    const size_t a_old = a;
    const size_t b_old = b;

    /* Call the function under verification */
    size_t result = aws_mul_size_saturating(a, b);

    /* Postcondition checks */
    bool overflow = (a_old != 0 && b_old > SIZE_MAX / a_old);
    if (!overflow) {
        assert(result == a_old * b_old);
    } else {
        assert(result == SIZE_MAX);
    }
    assert(result <= SIZE_MAX);
}
