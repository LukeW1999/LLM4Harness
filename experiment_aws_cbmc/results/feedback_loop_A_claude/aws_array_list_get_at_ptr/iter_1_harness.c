Looking at the function `aws_array_list_get_at_ptr`:

1. **What fields change on success?** `*val` is set to point to the element at `index`. The list itself is not modified.
2. **What fields are unchanged?** All fields of the list: `alloc`, `current_size`, `length`, `item_size`, `data`.
3. **What happens on failure?** Returns `AWS_OP_ERR`, list unchanged, `*val` is not written.
4. **What validity invariants must hold?** `aws_array_list_is_valid(list)` must hold after the call.

On success, `*val` should equal `(void*)((uint8_t*)list->data + list->item_size * index)`.

```c
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Output pointer */
    void *val;

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index < length, val points to element at index */
        assert(list.length > index);
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure: index >= length */
        assert(list.length <= index);
    }

    /* 7. List fields must not change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
