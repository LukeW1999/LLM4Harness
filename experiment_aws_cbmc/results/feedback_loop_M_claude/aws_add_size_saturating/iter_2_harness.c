#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Call the function under test */
    size_t result = aws_add_size_saturating(a, b);

    /* Postconditions */
    if (a <= SIZE_MAX - b) {
        /* No overflow: result should be a + b */
        assert(result == a + b);
    } else {
        /* Overflow: result should be saturated to SIZE_MAX */
        assert(result == SIZE_MAX);
    }
}
