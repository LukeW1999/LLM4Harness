#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_add_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Non-deterministic inputs */
    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result = aws_add_size_saturating(a, b);

    /* Postcondition 1: result is saturating - never exceeds SIZE_MAX */
    assert(result <= SIZE_MAX);

    /* Postcondition 2: if no overflow, result equals a + b */
    if (a <= SIZE_MAX - b) {
        assert(result == a + b);
    } else {
        /* Postcondition 3: on overflow, result is SIZE_MAX */
        assert(result == SIZE_MAX);
    }
}
