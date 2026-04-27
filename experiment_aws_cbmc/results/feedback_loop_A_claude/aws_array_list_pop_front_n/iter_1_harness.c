Looking at the function `aws_array_list_pop_front_n`:

1. **What fields change on success?**
   - `list->length` decreases by `n` (or becomes 0 if `n >= length`)
   - `list->data` contents shift (remaining elements move to front)

2. **What fields are unchanged?**
   - `list->alloc`, `list->current_size`, `list->item_size`, `list->data` pointer

3. **What happens on failure?**
   - This function returns void, no failure path. If `n >= length`, list is cleared (length = 0).

4. **What validity invariants must hold?**
   - `aws_array_list_is_valid(list)` must hold after the call.

Key behaviors:
- If `n >= length`: list is cleared, `length = 0`
- If `n == 0`: nothing changes
- If `0 < n < length`: `length = old_length - n`, data shifted

```c
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic n */
    size_t n;

    /* 3. Save old state */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Assert postconditions */

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Changed fields based on n vs old_length */
    if (n >= old_length) {
        /* List is cleared */
        assert(list.length == 0);
    } else if (n == 0) {
        /* Nothing changes */
        assert(list.length == old_length);
    } else {
        /* n > 0 and n < old_length */
        assert(list.length == old_length - n);
    }

    /* General: length must be <= old_length */
    assert(list.length <= old_length);

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
