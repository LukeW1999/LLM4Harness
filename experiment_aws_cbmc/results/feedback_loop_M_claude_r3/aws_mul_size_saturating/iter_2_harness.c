#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Non-deterministic inputs */
    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result = aws_mul_size_saturating(a, b);

    /* Postcondition: if multiplication would overflow, result must be SIZE_MAX */
    if (a != 0 && b > SIZE_MAX / a) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
