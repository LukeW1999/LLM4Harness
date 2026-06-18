#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    /* Non‑deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    int rc = aws_mul_size_saturating(a, b, &result);

    /* Compute overflow condition manually */
    bool overflow = (b != 0 && a > SIZE_MAX / b);

    /* The function should report overflow exactly when it occurs */
    assert((rc == 0) == !overflow);

    /* When no overflow, the result must equal the product */
    if (rc == 0) {
        assert(result == a * b);
    }
}
