// === STEP 1: SUCCESS PATH ===
// When aws_array_list_erase returns AWS_OP_SUCCESS:
//   - list->length: CHANGES to (old_length - 1)
//   - list->current_size: UNCHANGED (pop_back doesn't shrink)
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - list->data: UNCHANGED (same pointer, contents shifted)
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_erase returns AWS_OP_ERR (index >= length):
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - list->data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - current_size: UNCHANGED always
//     - length: CHANGED on success (decremented by 1), UNCHANGED on failure
//     - item_size: UNCHANGED always
//     - data: UNCHANGED (pointer itself), contents may shift on success
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold before and after call)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_array_list_erase_harness(void) {
    /* Declare and initialize the array list */
    struct aws_array_list list;

    /* Bound the list to make verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* item_size must be > 0 for meaningful operations */
    __CPROVER_assume(list.item_size > 0);

    /* Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;

    /* Nondeterministic index */
    size_t index;
    __CPROVER_assume(index <= old_length + 1); /* allow out-of-bounds too */

    /* Call the function under test */
    int result = aws_array_list_erase(&list, index);

    /* The list must remain valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions: these fields never change */
    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    if (result == AWS_OP_SUCCESS) {
        /* Success: index was valid (index < old_length) */
        assert(index < old_length);
        /* Length decremented by 1 */
        assert(list.length == old_length - 1);
        /* current_size unchanged */
        assert(list.current_size == old_list.current_size);
    } else {
        /* Failure: index was out of bounds */
        assert(result == AWS_OP_ERR);
        assert(index >= old_length);
        /* All fields unchanged */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
    }
}
