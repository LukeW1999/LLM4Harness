#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);
    size_t max = (size_t)-1;

    /* If addition does not overflow, result must equal the sum */
    if (a + b >= a) {
        assert(result == a + b);
    } else {
        /* Overflow case: result must be saturated to SIZE_MAX */
        assert(result == max);
    }

    /* Result must be at least as large as each operand */
    assert(result >= a);
    assert(result >= b);
}
