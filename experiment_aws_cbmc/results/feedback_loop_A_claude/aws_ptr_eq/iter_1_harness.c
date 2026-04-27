Looking at the function `aws_ptr_eq`, from the header:

```c
bool aws_ptr_eq(const void *a, const void *b);
```

And the Doxygen comment:
> Equality function which compares pointer equality.

This is a simple function that compares two pointers for equality and returns `true` if they are equal, `false` otherwise.

**Analysis:**
1. **Changed fields on success**: Returns `true` if `a == b`, `false` otherwise. No data structures are modified.
2. **Unchanged fields**: No structures involved - just two pointer arguments.
3. **Failure path**: N/A - this function always succeeds (returns bool, not int).
4. **Validity invariants**: No structures to validate.

The postconditions are:
- If `a == b`, result must be `true`
- If `a != b`, result must be `false`
- Equivalently: `result == (a == b)`

```c
#include <aws/common/hash_table.h>
#include <aws/common/common.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ptr_eq_harness(void) {
    /* 1. Declare inputs - two arbitrary pointers */
    const void *a;
    const void *b;

    /* 2. Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* 3. Assert postconditions */
    /* The function compares pointer equality */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* Equivalently, the result must exactly match pointer equality */
    assert(result == (a == b));

    /* The input pointers must not have been modified */
    /* (pointers are passed by value, so they cannot be modified) */
    /* No structures to validate for validity invariants */
}
