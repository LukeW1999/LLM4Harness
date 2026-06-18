#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result = aws_mul_size_saturating(a, b);

    if (a != 0 && b != 0) {
        if (SIZE_MAX / a < b) {
            assert(result == SIZE_MAX);
        } else {
            assert(result == a * b);
        }
    } else {
        assert(result == 0);
    }
}
