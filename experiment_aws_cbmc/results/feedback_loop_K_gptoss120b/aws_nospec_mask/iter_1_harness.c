/*  
 * Contract for aws_nospec_mask  
 * Preconditions: none (index and bound may be any size_t values)  
 * Postconditions (validity): the returned mask is either 0 or UINTPTR_MAX.  
 * Postconditions (semantic):  
 *   - If index < bound && index <= (SIZE_MAX/2) && bound <= (SIZE_MAX/2) then  
 *       mask == UINTPTR_MAX.  
 *   - Otherwise mask == 0.  
 * Postconditions (frame): the function does not modify any memory locations.  
 */

#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Declaration of the function under test */
size_t aws_nospec_mask(size_t index, size_t bound);

void aws_nospec_mask_harness(void) {
    /* nondeterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* No additional assumptions needed */

    size_t mask = aws_nospec_mask(index, bound);

    /* The mask must be either all‑zeros or all‑ones */
    assert(mask == 0 || mask == UINTPTR_MAX);

    /* Derive the expected condition */
    bool in_range = false;
    if (index < bound &&
        index <= (SIZE_MAX / 2) &&
        bound <= (SIZE_MAX / 2)) {
        in_range = true;
    }

    /* Relate the result to the condition */
    if (in_range) {
        assert(mask == UINTPTR_MAX);
    } else {
        assert(mask == 0);
    }

    return 0;
}
