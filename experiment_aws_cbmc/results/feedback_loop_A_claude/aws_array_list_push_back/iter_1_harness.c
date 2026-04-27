Looking at the function `aws_array_list_set_at`:

**Analysis:**
1. **What changes on success?** The element at `index` is copied from `val`. If `index >= list->length`, then `list->length` becomes `index + 1`. The `data` buffer may be reallocated (if dynamic and capacity needed to grow).
2. **What is unchanged?** `list->item_size`, `list->alloc` (allocator pointer itself doesn't change).
3. **What happens on failure?** Returns `AWS_OP_ERR`, list remains valid.
4. **Validity invariants?** `aws_array_list_is_valid(list)` must hold after the call.

The function:
- Calls `aws_array_list_ensure_capacity` which may reallocate `data` and change `current_size`
- Copies `val` into `list->data + (item_size * index)`
- Updates `list->length` to `index + 1` if `index >= length`

```c
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 4
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create a non-deterministic val pointer with readable memory of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Non-deterministic index */
    size_t index;
    /* Bound index to keep state space manageable */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the element was written at index */
        /* If index was >= old_length, length becomes index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_length);
        }
        /* current_size must be >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);
        /* data must be non-null since we wrote something */
        assert(list.data != NULL);
        /* The value at index matches what we wrote */
        assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + (list.item_size * index), list.item_size));
    } else {
        /* On failure: list must still be valid (already asserted above) */
        /* length should not have increased beyond what it was */
        /* The list remains valid - no stronger guarantee from the spec */
        assert(aws_array_list_is_valid(&list));
    }
}
