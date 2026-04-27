#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_add_size_saturating_harness() {
    /* 1. Declare data structure(s) on stack */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* 2. Bound the structure (limits CBMC state space) */
    /* No need to bound scalar types */

    /* 3. Allocate pointer members */
    /* No pointer members to allocate */

    /* 4. Assume validity precondition (infer from how function uses the struct) */
    /* No specific preconditions needed for scalar types */

    /* 5. Save old state (for checking immutability) */
    /* No state to save for scalar types */

    /* 6. Assume function-specific preconditions (infer from code logic) */
    /* No additional preconditions needed for this function */

    /* 7. Call function under test */
    aws_add_size_saturating(a, b, r_ptr);

    /* 8. Assert postconditions (infer from what the function guarantees) */
    if (a <= SIZE_MAX - b) {
        assert(r == a + b); /* On success, r should contain the sum of a and b */
    } else {
        assert(r == SIZE_MAX); /* On failure, r should be saturated to SIZE_MAX */
    }
}
