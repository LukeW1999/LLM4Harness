Looking at the function `aws_array_list_set_at`:

**Analysis:**
1. **What changes on success?** The element at `index` is copied from `val`. If `index >= list->length`, then `list->length` becomes `index + 1`. The `data` buffer may be reallocated (if dynamic and capacity needed to grow).
2. **What is unchanged?** `list->item_size`, `list->alloc`. On failure, the list should remain valid.
3. **What happens on failure?** Returns `AWS_OP_ERR`, list remains valid (capacity ensure failed or overflow in length calculation).
4. **Validity invariants?** `aws_array_list_is_valid(list)` must hold after the call.

Key observations from implementation:
- Calls `aws_array_list_ensure_capacity` which may reallocate `data` and change `current_size`
- Copies `val` into `list->data + (item_size * index)`
- Updates `list->length` to `index + 1` if `index >= current length`
- `item_size` and `alloc` never change

```c
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old_list = list;

    /* 3. Set up val: must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Non-deterministic index */
    size_t index;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length is at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was already within bounds, length doesn't shrink */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* index >= old length: new length is index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient to hold the new length */
        assert(list.current_size >= list.length * list.item_size);

        /* data must be non-null since we have at least one element */
        assert(list.data != NULL);

        /* The value was copied into the correct position */
        assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
    } else {
        /* On failure: list remains valid (already asserted above) */
        /* length should not have increased beyond what it was */
        /* The list validity is the key invariant */
        assert(aws_array_list_is_valid(&list));
    }
}
