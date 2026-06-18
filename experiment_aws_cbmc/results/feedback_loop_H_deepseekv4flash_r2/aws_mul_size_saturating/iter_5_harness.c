#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness() {
    size_t a;
    size_t b;
    /* Bound inputs to keep verification time reasonable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t result = aws_mul_size_saturating(a, b);

    /* Specification: result == a*b if no overflow, else SIZE_MAX */
    if (__CPROVER_overflow_multiply(a, b)) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
