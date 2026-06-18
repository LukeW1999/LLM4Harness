#include <aws/common/math.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;

    bool overflow = aws_add_size_saturating(a, b, &result);

    assert(overflow == (a > ((size_t)-1) - b));

    if (overflow) {
        assert(result == (size_t)-1);
    } else {
        assert(result == a + b);
    }
}
