#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <limits.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    size_t old_a = a;
    size_t old_b = b;

    int rc = aws_mul_size_saturating(a, b, &result);

    if (rc == AWS_OP_SUCCESS) {
        if (a != 0) {
            assert(result / a == b);
        }
    } else {
        assert(result == SIZE_MAX);
    }

    assert(a == old_a);
    assert(b == old_b);
}
