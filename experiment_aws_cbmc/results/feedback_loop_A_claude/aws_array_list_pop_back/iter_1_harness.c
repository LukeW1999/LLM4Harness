Looking at the function `aws_array_list_set_at`, I need to analyze:

1. **What changes on success**: The element at `index` is set to `val`, and if `index >= list->length`, then `list->length` becomes `index + 1`.
2. **What's unchanged**: `list->item_size`, `list->alloc`, `list->current_size` (unless capacity grows via `ensure_capacity`), but `current_size` can change if dynamic reallocation happens.
3. **What happens on failure**: Returns `AWS_OP_ERR`, list remains valid.
4. **Validity invariants**: `aws_array_list_is_valid(list)` must hold after the call.

Key observations from the implementation:
- It calls `aws_array_list_ensure_capacity` which may grow the list (changing `data` and `current_size`)
- On success, `list->length` is updated if `index >= old length`
- `item_size` and `alloc` never change
- The val is copied into the list at the given index

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

    /* 2. Create a non-deterministic val of item_size bytes */
    /* val must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Non-deterministic index */
    size_t index;

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

        /* current_size must be >= length * item_size (capacity was ensured) */
        size_t required_size = list.length * list.item_size;
        assert(list.current_size >= required_size);

        /* The data at index must match val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

    } else {
        /* On failure, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On failure from ensure_capacity, length should not have changed */
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
