#include <aws/common/math.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;
    size_t old_a = a;
    size_t old_b = b;

    bool overflow = aws_mul_size_checked(a, b, &result);

    if (!overflow) {
        assert(result == a * b);
    }

    assert(a == old_a);
    assert(b == old_b);
}
