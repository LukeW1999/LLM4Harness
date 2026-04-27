Looking at the function `aws_array_list_set_at`:

**Analysis:**
1. **What changes on success?** The element at `index` is copied from `val`. If `index >= list->length`, then `list->length` becomes `index + 1`. The `list->data` pointer may change (if `ensure_capacity` reallocates). `list->current_size` may change.
2. **What's unchanged?** `list->item_size`, `list->alloc`
3. **What happens on failure?** Returns `AWS_OP_ERR`, list remains valid (postcondition says `aws_array_list_is_valid`)
4. **Validity invariants?** `aws_array_list_is_valid(list)` must hold after the call

The function:
- Calls `ensure_capacity` which may grow the list (dynamic mode)
- Copies `val` into `list->data + item_size * index`
- Updates `list->length` if `index >= length`

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
    /* 1. Declare and set up the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up val: must be readable for item_size bytes */
    void *val = malloc(list.item_size);
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
    size_t old_current_size = list.current_size;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the element was written, length updated if needed */
        /* If index was >= old_length, new length = index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_length);
        }
        /* current_size must be >= length * item_size */
        size_t required_size = list.length * list.item_size;
        assert(list.current_size >= required_size);
        /* data must be non-null since we wrote something */
        assert(list.data != NULL);
        /* The value at index matches val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        void *stored = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(memcmp(stored, val, list.item_size) == 0);
    } else {
        /* On failure: list remains valid (already checked above) */
        /* length should not have increased beyond what it was */
        assert(list.length <= old_length || list.length == index + 1);
        /* item_size unchanged */
        assert(list.item_size == old_item_size);
        /* alloc unchanged */
        assert(list.alloc == old_alloc);
    }
}
