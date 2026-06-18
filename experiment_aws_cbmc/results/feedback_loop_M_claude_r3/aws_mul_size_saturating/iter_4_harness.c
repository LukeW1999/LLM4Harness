#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    __CPROVER_assume(a <= 0xFFFF);
    __CPROVER_assume(b <= 0xFFFF);

    size_t result = aws_mul_size_saturating(a, b);

    /* Postcondition: if multiplication would overflow, result must be SIZE_MAX */
    if (a != 0 && b > SIZE_MAX / a) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
