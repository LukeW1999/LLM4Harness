#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

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
        /* Success: index < length, val points into data at correct offset */
        assert(list.length > index);
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure: index >= length, val is unchanged (not written) */
        assert(list.length <= index);
    }

    /* 7. Assert unchanged fields — function never modifies the list itself */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Assert validity invariant still holds */
    assert(aws_array_list_is_valid(&list));
}
