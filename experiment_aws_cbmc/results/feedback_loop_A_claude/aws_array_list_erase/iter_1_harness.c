Looking at the function `aws_array_list_erase`:

1. **What fields change on success?** `list->length` decreases by 1 (via pop_front or pop_back). The data contents shift (elements after index move forward).
2. **What fields are unchanged?** `list->alloc`, `list->item_size`, `list->current_size`, `list->data` pointer (data pointer itself doesn't change).
3. **What happens on failure?** Returns `AWS_OP_ERR` when `index >= length`. List is unchanged.
4. **Validity invariants?** `aws_array_list_is_valid(list)` must hold after the call.

```c
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* Save a byte from the data for checking data integrity on failure */
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (< old length) */
        assert(index < old.length);
        /* Length decreases by 1 */
        assert(list.length == old.length - 1);
        /* These fields must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* On failure: index must have been out of bounds */
        assert(index >= old.length);
        /* List must be completely unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* Data contents unchanged */
        if (list.current_size > 0 && list.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
