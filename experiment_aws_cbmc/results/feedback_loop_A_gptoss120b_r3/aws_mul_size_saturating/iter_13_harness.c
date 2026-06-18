#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();
    size_t old_result = (size_t)nondet_uint64_t();
    size_t result = old_result;

    int rc = aws_mul_size_saturating(a, b, &result);

    if (rc == AWS_OP_SUCCESS) {
        assert(a == 0 || b <= ((size_t)-1) / a);
        assert(result == a * b);
    } else {
        assert(result == old_result);
    }
}
