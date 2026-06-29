#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "aws/common/private/byte_buf.h"

void aws_nospec_mask_harness(void) {
    size_t index;
    size_t bound;

    /* Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /*
     * Postcondition 1: Return value correctness
     * The function returns UINTPTR_MAX if index < bound AND
     * index <= (SIZE_MAX / 2) AND bound <= (SIZE_MAX / 2),
     * otherwise returns 0.
     */
    bool index_in_range = (index < bound);
    bool index_no_sign = (index <= (SIZE_MAX / 2));
    bool bound_no_sign = (bound <= (SIZE_MAX / 2));

    if (index_in_range && index_no_sign && bound_no_sign) {
        /* Should return UINTPTR_MAX (all ones) */
        assert(result == UINTPTR_MAX);
    } else {
        /* Should return 0 */
        assert(result == 0);
    }

    /*
     * Postcondition 2: Result is either 0 or UINTPTR_MAX (no other values)
     */
    assert(result == 0 || result == UINTPTR_MAX);

    /*
     * Postcondition 3: If result is UINTPTR_MAX, then index must be strictly
     * less than bound (safe to use as array index into array of size bound)
     */
    if (result == UINTPTR_MAX) {
        assert(index < bound);
        assert(index <= (SIZE_MAX / 2));
        assert(bound <= (SIZE_MAX / 2));
    }

    /*
     * Postcondition 4: If index >= bound, result must be 0
     */
    if (index >= bound) {
        assert(result == 0);
    }

    /*
     * Postcondition 5: If bound > SIZE_MAX / 2, result must be 0
     */
    if (bound > (SIZE_MAX / 2)) {
        assert(result == 0);
    }

    /*
     * Postcondition 6: If index > SIZE_MAX / 2, result must be 0
     */
    if (index > (SIZE_MAX / 2)) {
        assert(result == 0);
    }

    /*
     * Postcondition 7: Frame condition - the function is pure (no side effects),
     * so index and bound are not modified. We verify this by checking that
     * calling the function again with the same inputs yields the same result.
     */
    size_t result2 = aws_nospec_mask(index, bound);
    assert(result == result2);

    /*
     * Postcondition 8: AND-ing result into index gives a safe index
     * If result == 0, then (index & result) == 0, which is a safe index
     * If result == UINTPTR_MAX, then (index & result) == index, which is < bound
     */
    size_t masked_index = index & result;
    if (result == UINTPTR_MAX) {
        assert(masked_index == index);
        assert(masked_index < bound);
    } else {
        assert(masked_index == 0);
    }
}
