#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/* Forward declaration of the function under test */
uintptr_t aws_nospec_mask(size_t index, size_t bound);

void aws_nospec_mask_harness(void) {
    /* 1. Declare non-deterministic inputs */
    size_t index;
    size_t bound;

    /* 2. Call function under test */
    uintptr_t result = aws_nospec_mask(index, bound);

    /* 3. Assert postconditions */

    /* The result must always be either 0 or UINTPTR_MAX */
    assert(result == 0 || result == UINTPTR_MAX);

    /* If index >= bound, result must be 0 */
    if (index >= bound) {
        assert(result == 0);
    }

    /* If bound > (SIZE_MAX / 2), result must be 0 */
    if (bound > (SIZE_MAX / 2)) {
        assert(result == 0);
    }

    /* If index > (SIZE_MAX / 2), result must be 0 */
    if (index > (SIZE_MAX / 2)) {
        assert(result == 0);
    }

    /* If none of the failure conditions hold, result must be UINTPTR_MAX */
    if (index < bound && bound <= (SIZE_MAX / 2) && index <= (SIZE_MAX / 2)) {
        assert(result == UINTPTR_MAX);
    }

    /* Contrapositive: if result == UINTPTR_MAX, none of the failure conditions hold */
    if (result == UINTPTR_MAX) {
        assert(index < bound);
        assert(bound <= (SIZE_MAX / 2));
        assert(index <= (SIZE_MAX / 2));
    }

    /* Contrapositive: if result == 0, at least one failure condition holds */
    if (result == 0) {
        assert(index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2));
    }
}
