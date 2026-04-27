Looking at the function `aws_array_list_set_at`, let me analyze:

1. **What fields change on success?**
   - `list->data` contents at `index * item_size` are overwritten with `val`
   - `list->length` may increase to `index + 1` if `index >= current length`
   - `list->current_size` may increase (via `ensure_capacity`)

2. **What fields are unchanged?**
   - `list->alloc` - allocator unchanged
   - `list->item_size` - item size unchanged

3. **What happens on failure?**
   - Returns `AWS_OP_ERR`, list may be partially modified (ensure_capacity failed)

4. **Validity invariants?**
   - `aws_array_list_is_valid(list)` must hold after the call

```c
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create a non-deterministic val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Non-deterministic index */
    size_t index;

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* item_size must not change */
        assert(list.item_size == old_list.item_size);

        /* allocator must not change */
        assert(list.alloc == old_list.alloc);

        /* data pointer must be non-null after successful set */
        assert(list.data != NULL);

        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old length, length stays the same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be large enough to hold index+1 elements */
        assert(list.current_size >= list.length * list.item_size);

        /* The data at index must match val */
        assert(memcmp((uint8_t *)list.data + (list.item_size * index), val, list.item_size) == 0);

        /* current_size must not shrink */
        assert(list.current_size >= old_list.current_size);

    } else {
        /* On failure: item_size and alloc must not change */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
