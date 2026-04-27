#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_add_size_saturating_harness() {
    /* 1. Declare data structure(s) on stack */
    size_t a;
    size_t b;
    size_t r;

    /* 2. Bound the structure (limits CBMC state space) */
    a = nondet_size_t();
    b = nondet_size_t();

    /* 3. Allocate pointer members */
    /* No pointer members to allocate for size_t */

    /* 4. Assume validity precondition */
    /* No specific validity preconditions for size_t inputs */

    /* 5. Save old state (for checking immutability) */
    /* No state to save for size_t inputs */

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    /* No specific function-specific preconditions mentioned */

    /* 7. Call function under test */
    aws_add_size_saturating(a, b, &r);

    /* 8. Assert postconditions (from Ensures: annotations) */
    if (a <= SIZE_MAX - b) {
        assert(r == a + b); /* If no overflow, the result is the sum of a and b */
    } else {
        assert(r == SIZE_MAX); /* If overflow, the result is SIZE_MAX */
    }
}
