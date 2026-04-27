Looking at the function `aws_array_list_set_at`:

**Analysis:**
1. **What changes on success?** The element at `index` is copied from `val`. If `index >= list->length`, then `list->length` becomes `index + 1`. The `data` buffer may be reallocated (if dynamic and capacity needed to grow).
2. **What is unchanged?** `list->item_size`, `list->alloc` (allocator pointer itself doesn't change).
3. **What happens on failure?** Returns `AWS_OP_ERR`, list remains valid.
4. **Validity invariants?** `aws_array_list_is_valid(list)` must hold after the call.

The function copies `val` into `list->data` at position `index`, and if `index >= length`, updates `length = index + 1`. It may grow capacity in dynamic mode.

```c
#include <aws/common/array_list.h>
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

    /* 2. Save old state */
    struct aws_array_list old_list = list;

    /* 3. Prepare val: readable memory of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Non-deterministic index */
    size_t index;
    /* Bound index to keep state space manageable */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_list.item_size);

    /* allocator never changes */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: if index was >= old length, new length = index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_list.length);
        }

        /* The data at index must match val */
        assert(AWS_MEM_IS_READABLE(list.data, list.item_size * list.length));
        void *stored = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(memcmp(stored, val, list.item_size) == 0);

        /* current_size must be at least enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);

    } else {
        /* On failure: list remains valid (already asserted above) */
        /* length should not have increased beyond what's valid */
        assert(list.length <= old_list.length || list.length == index + 1);

        /* item_size and alloc unchanged (already asserted) */
    }
}
