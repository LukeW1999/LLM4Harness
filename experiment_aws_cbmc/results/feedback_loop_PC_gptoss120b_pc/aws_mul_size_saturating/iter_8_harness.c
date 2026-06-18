#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = (size_t)__CPROVER_nondet_unsigned_long();
    size_t b = (size_t)__CPROVER_nondet_unsigned_long();

    size_t old_result = (size_t)__CPROVER_nondet_unsigned_long();
    size_t result = old_result;

    int ret = aws_mul_size_saturating(a, b, &result);

    if (ret == 0) {
        assert(result == a * b);
        if (a != 0) {
            assert(result / a == b);
        }
    } else {
        assert(result == old_result);
        assert(a != 0 && b != 0 && ((size_t)-1 / a) < b);
    }
}
