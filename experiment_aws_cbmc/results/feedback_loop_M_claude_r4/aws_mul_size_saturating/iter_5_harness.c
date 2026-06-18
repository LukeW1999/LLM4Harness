#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    __CPROVER_assume(a <= 0xFFFF);
    __CPROVER_assume(b <= 0xFFFF);

    /* Call the function under test */
    size_t result = aws_mul_size_saturating(a, b);

    /* Postconditions */
    if (a != 0 && b != 0 && a > SIZE_MAX / b) {
        /* Overflow case: result should be saturated to SIZE_MAX */
        assert(result == SIZE_MAX);
    } else {
        /* No overflow: result should equal a * b */
        assert(result == a * b);
    }
}
