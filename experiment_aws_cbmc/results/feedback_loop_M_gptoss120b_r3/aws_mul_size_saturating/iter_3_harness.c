#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    int rc = aws_mul_size_saturating(a, b, &result);

    bool overflow = (b != 0 && a > SIZE_MAX / b);

    assert((rc == 0) == !overflow);

    if (rc == 0) {
        assert(result == a * b);
    }
}
