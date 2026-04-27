Looking at the function `aws_array_list_set_at`:

**Analysis:**
1. **What changes on success?** The element at `index` is copied from `val`. If `index >= list->length`, then `list->length` becomes `index + 1`. The `data` buffer may be reallocated (if dynamic and capacity needed to grow).
2. **What is unchanged?** `list->item_size`, `list->alloc`. On failure, the list should remain valid.
3. **What happens on failure?** Returns `AWS_OP_ERR`, list remains valid (capacity ensure failed or overflow in length calculation).
4. **Validity invariants?** `aws_array_list_is_valid(list)` must hold after the call.

Key observations from implementation:
- Calls `aws_array_list_ensure_capacity` which may reallocate `data` and change `current_size`
- On success, `length` may increase to `index + 1` if `index >= old_length`
- `item_size` and `alloc` never change
- The `val` pointer must be readable for `item_size` bytes

```c
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Non-deterministic val - must be readable for item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* item_size and alloc never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must be valid */
        assert(aws_array_list_is_valid(&list));

        /* If index was within old length, length stays the same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* If index >= old length, length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be at least enough to hold index+1 items */
        assert(list.current_size >= list.length * list.item_size);

        /* The data at index must match val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));

    } else {
        /* On failure, list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On failure, length should not have increased beyond what it was */
        /* The ensure_capacity failed, so data/current_size/length should be unchanged
           or at most unchanged (ensure_capacity may have failed without modifying) */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
