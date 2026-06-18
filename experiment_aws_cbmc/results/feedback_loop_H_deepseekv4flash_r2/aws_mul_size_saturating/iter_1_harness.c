#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);

    /* Check postcondition: result is either a+b (no overflow) or SIZE_MAX (overflow) */
    size_t sum = a + b;
    if (sum < a) {
        /* Overflow occurred in plain addition */
        assert(result == SIZE_MAX);
    } else {
        /* No overflow */
        assert(result == sum);
    }

    /* Additional consistency: if result == SIZE_MAX then overflow must have occurred */
    if (result == SIZE_MAX) {
        size_t sum2 = a + b;
        assert(sum2 < a);
    }

    /* If result is not SIZE_MAX, it must equal the sum */
    if (result != SIZE_MAX) {
        assert(result == a + b);
    }
}
