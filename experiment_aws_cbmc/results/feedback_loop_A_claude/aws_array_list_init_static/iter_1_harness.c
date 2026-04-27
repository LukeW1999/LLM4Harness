Looking at the function `aws_array_list_init_static`, I need to analyze:

1. **What fields change**: The function initializes all fields of the list: `alloc = NULL`, `current_size = item_count * item_size`, `item_size = item_size`, `length = 0`, `data = raw_array`.

2. **What fields are unchanged**: N/A - this is an initialization function, all fields are set.

3. **What happens on failure**: The function uses `AWS_FATAL_PRECONDITION` (aborts on failure), so no failure path to handle - it either succeeds or aborts.

4. **Validity invariants**: After the call, `aws_array_list_is_valid(list)` must hold.

The preconditions from `AWS_FATAL_PRECONDITION`:
- `list != NULL`
- `raw_array != NULL`
- `item_count > 0`
- `item_size > 0`
- `item_count * item_size` must not overflow

```c
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;

    size_t item_count;
    size_t item_size;

    /* Preconditions from AWS_FATAL_PRECONDITION */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    /* Ensure item_count * item_size does not overflow */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* Bound the sizes to keep the state space manageable */
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Allocate raw_array with sufficient size */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 2. Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 3. Assert postconditions */

    /* Changed fields - explicitly set by the function */
    assert(list.alloc == NULL);
    assert(list.current_size == current_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* 4. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));
}
