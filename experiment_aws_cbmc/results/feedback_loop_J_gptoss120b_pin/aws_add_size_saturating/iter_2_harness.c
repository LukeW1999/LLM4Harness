#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = 0;

    bool overflow = aws_add_size_saturating(a, b, &result);

    /* The function should indicate overflow exactly when a + b would exceed SIZE_MAX */
    assert(overflow == (a > SIZE_MAX - b));

    if (overflow) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a + b);
    }
}
