#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <limits.h>
#include <assert.h>

void aws_mul_size_saturating_harness() {
    uint64_t a64 = nondet_uint64_t();
    uint64_t b64 = nondet_uint64_t();
    __CPROVER_assume(a64 <= SIZE_MAX);
    __CPROVER_assume(b64 <= SIZE_MAX);
    size_t a = (size_t)a64;
    size_t b = (size_t)b64;

    size_t result;
    int rc = aws_mul_size_saturating(a, b, &result);

    if (rc == 0) {
        if (a != 0) {
            assert(result / a == b);
        } else {
            assert(result == 0);
        }
        assert(b == 0 || a <= SIZE_MAX / b);
    } else {
        assert(!(b == 0 || a <= SIZE_MAX / b));
    }
}
