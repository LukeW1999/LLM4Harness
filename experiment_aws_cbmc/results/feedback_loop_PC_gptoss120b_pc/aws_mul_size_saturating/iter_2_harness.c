#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <limits.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();

    size_t old_result = __CPROVER_nondet_size_t();
    size_t result = old_result;

    int ret = aws_mul_size_saturating(a, b, &result);

    if (ret == 0) {
        /* No overflow: the product must be exact */
        assert(result == a * b);
        if (a != 0) {
            assert(result / a == b);
        }
    } else {
        /* Overflow: result must be unchanged */
        assert(result == old_result);
        /* The overflow condition can be expressed as: */
        assert(a != 0 && b != 0 && (SIZE_MAX / a) < b);
    }
}
