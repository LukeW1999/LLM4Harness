Looking at the function `aws_array_list_pop_front`:

1. **What fields change on success?** `length` decreases by 1 (one element removed from front, remaining elements shifted). The `data` contents shift but the pointer itself doesn't change.
2. **What fields are unchanged?** `alloc`, `current_size`, `item_size`, `data` pointer.
3. **What happens on failure?** List is empty (`length == 0`), returns `AWS_OP_ERR`, list unchanged.
4. **Validity invariants?** `aws_array_list_is_valid` must hold after the call.

```c
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_pop_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list was non-empty, length decreases by 1 */
        assert(old.length > 0);
        assert(list.length == old.length - 1);
    } else {
        /* Failure: list was empty, nothing changes */
        assert(old.length == 0);
        assert(list.length == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
