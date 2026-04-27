// === STEP 1: SUCCESS PATH ===
// When aws_array_list_pop_back returns AWS_OP_SUCCESS:
//   - list->length: CHANGES to (old_length - 1)
//   - list->data: UNCHANGED (pointer itself unchanged, but memory at last_item_offset is zeroed)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - The memory at (list->data + last_item_offset) is zeroed (item_size bytes)
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
// list (struct aws_array_list):
//   - length: CHANGED on success (decremented by 1), UNCHANGED on failure
//   - data: UNCHANGED always (pointer value)
//   - current_size: UNCHANGED always
//   - item_size: UNCHANGED always
//   - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call, postcondition)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_array_list_pop_back_harness(void) {
    /* Allocate and initialize a valid array list */
    struct aws_array_list list;

    /* Use bounded list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;

    /* Call the function under test */
    int result = aws_array_list_pop_back(&list);

    /* === STEP 4: Validity invariant === */
    assert(aws_array_list_is_valid(&list));

    /* === STEP 3: Frame conditions - always unchanged === */
    assert(list.data == old_list.data);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* === STEP 1: Success path === */
        /* Precondition for success: old_length > 0 */
        assert(old_length > 0);

        /* length decremented by 1 */
        assert(list.length == old_length - 1);

        /* The memory at last_item_offset should be zeroed */
        size_t last_item_offset = list.item_size * (old_length - 1);
        uint8_t *last_item_ptr = (uint8_t *)list.data + last_item_offset;
        for (size_t i = 0; i < list.item_size; i++) {
            assert(last_item_ptr[i] == 0);
        }
    } else {
        /* === STEP 2: Failure path === */
        /* Failure means list was empty */
        assert(old_length == 0);

        /* length unchanged */
        assert(list.length == old_list.length);
    }
}
