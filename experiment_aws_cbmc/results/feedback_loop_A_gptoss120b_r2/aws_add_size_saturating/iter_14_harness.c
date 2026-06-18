#include <stddef.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = 0;
    int ret = aws_add_size_saturating(a, b, &result);

    if (a > (size_t)-1 - b) {
        __CPROVER_assert(ret != 0, "ret non-zero on overflow");
        __CPROVER_assert(result == (size_t)-1, "result max on overflow");
    } else {
        __CPROVER_assert(ret == 0, "ret zero on no overflow");
        __CPROVER_assert(result == a + b, "result sum on no overflow");
    }
}
