#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_saturating_harness(void) {
    size_t a;
    size_t b;

    size_t result = aws_add_size_saturating(a, b);

    /* If the addition would overflow, result should be SIZE_MAX */
    if (a > SIZE_MAX - b) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a + b);
    }
}
