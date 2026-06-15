#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <limits.h>

void aws_add_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t result;
    int rc = aws_add_size_saturating(a, b, &result);

    if (a <= SIZE_MAX - b) {
        __CPROVER_assert(rc == 0, "rc should be zero when no overflow");
        __CPROVER_assert(result == a + b, "result should be sum when no overflow");
    } else {
        __CPROVER_assert(rc != 0, "rc should be non-zero on overflow");
        __CPROVER_assert(result == SIZE_MAX, "result should be SIZE_MAX on overflow");
    }
}
