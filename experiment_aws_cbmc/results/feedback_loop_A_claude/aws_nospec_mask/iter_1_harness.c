Looking at the function `aws_nospec_mask(size_t index, size_t bound)`:

**Analysis:**
1. **What changes on success?** The function returns a mask value - either 0 or UINTPTR_MAX based on conditions.
2. **What is unchanged?** The inputs `index` and `bound` are not modified (they're passed by value).
3. **What happens on failure?** Returns 0 when: `index >= bound`, `bound > (SIZE_MAX / 2)`, or `index > (SIZE_MAX / 2)`.
4. **Validity invariants:** The return value must be either 0 or UINTPTR_MAX.

**Postconditions from Doxygen:**
- If `index >= bound`: returns 0
- If `bound > (SIZE_MAX / 2)`: returns 0  
- If `index > (SIZE_MAX / 2)`: returns 0
- Otherwise: returns UINTPTR_MAX

```c
#include <aws/common/private/byte_buf.h>
#include <aws/common/byte_buf.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_nospec_mask_harness(void) {
    /* 1. Declare non-deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call function under test */
    size_t result = aws_nospec_mask(index, bound);

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
