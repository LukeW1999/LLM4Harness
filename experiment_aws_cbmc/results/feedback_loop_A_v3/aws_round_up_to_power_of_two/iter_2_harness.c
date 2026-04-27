#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare data structure(s) on stack */
    size_t n;
    size_t result;

    /* 2. Bound the structure (limits CBMC state space) */
    n = nondet_size_t();

    /* 3. Allocate pointer members */
    /* No pointer members to allocate for this function */

    /* 4. Assume validity precondition */
    /* No specific validity precondition needed for primitive types */

    /* 5. Save old state (for checking immutability) */
    /* No state to save for this function */

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    /* No specific preconditions mentioned in the Requires section for this function */

    /* 7. Call function under test */
    result = aws_round_up_to_power_of_two(n);

    /* 8. Assert postconditions (from Ensures: annotations) */
    /* Ensure result is a power of two */
    assert((result & (result - 1)) == 0);
    /* Ensure result is greater than or equal to n */
    assert(result >= n);
}
