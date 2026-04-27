// === STEP 1: SUCCESS PATH ===
// When aws_array_list_pop_back returns AWS_OP_SUCCESS:
//   - list->length: CHANGES to (old_length - 1)
//   - list->data: UNCHANGED (pointer itself unchanged, but memory at last item offset is zeroed)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_pop_back returns AWS_OP_ERR (list is empty, length == 0):
//   - list->length: UNCHANGED (still 0)
//   - list->data: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - length: CHANGED on success (decremented by 1), UNCHANGED on failure
//     - data: UNCHANGED (pointer value; content at last slot zeroed on success)
//     - current_size: UNCHANGED always
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_pop_back_harness(void) {
    /* Declare and initialize the array list */
    struct aws_array_list list;

    /* Use bounded list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Call the function under test */
    int result = aws_array_list_pop_back(&list);

    /* === STEP 4: Validity invariant === */
    assert(aws_array_list_is_valid(&list));

    /* === STEP 3: Frame conditions - always unchanged fields === */
    assert(list.data == old_list.data);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* === STEP 1: Success path === */
        /* List was non-empty (old_list.length > 0) */
        assert(old_list.length > 0);
        /* length decremented by 1 */
        assert(list.length == old_list.length - 1);
    } else {
        /* === STEP 2: Failure path === */
        /* List was empty */
        assert(old_list.length == 0);
        /* length unchanged */
        assert(list.length == old_list.length);
        /* result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
