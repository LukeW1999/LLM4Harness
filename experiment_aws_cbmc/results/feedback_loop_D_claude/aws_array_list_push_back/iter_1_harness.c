// === STEP 1: SUCCESS PATH ===
// When aws_array_list_push_back returns AWS_OP_SUCCESS:
//   - list->length: CHANGES to old_length + 1 (set_at increments length when index >= current length)
//   - list->data: MAY CHANGE (if reallocation occurred due to dynamic growth)
//   - list->current_size: MAY CHANGE (if reallocation occurred)
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - The element at index old_length is set to *val
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_push_back returns AWS_OP_ERR:
//   - This happens when aws_array_list_set_at fails AND either:
//     a) error is AWS_ERROR_INVALID_INDEX and list is static (no alloc) -> returns AWS_ERROR_LIST_EXCEEDS_MAX_SIZE
//     b) some other error from set_at
//   - list->length: UNCHANGED (set_at failed before modifying length)
//   - list->data: UNCHANGED (no successful reallocation)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - data: CHANGED on success (possible realloc), UNCHANGED on failure
//   - length: CHANGED on success (incremented by 1), UNCHANGED on failure
//   - current_size: CHANGED on success (possible realloc), UNCHANGED on failure
//   - item_size: UNCHANGED always
//   - alloc: UNCHANGED always
// val (const void *): UNCHANGED always (read-only input)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(list): YES (postcondition holds after call in both paths)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

void aws_array_list_push_back_harness() {
    /* Declare and initialize the list */
    struct aws_array_list list;

    /* Bound the list to make verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Create a valid val pointer with item_size readable bytes */
    const void *val;
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Call the function under test */
    int result = aws_array_list_push_back(&list, val);

    /* === STEP 4: Validity invariant === */
    assert(aws_array_list_is_valid(&list));

    /* === STEP 3: Frame conditions === */
    /* item_size and alloc are always unchanged */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* === STEP 1: Success path === */
        /* length must have increased by 1 */
        assert(list.length == old_list.length + 1);
        /* current_size must be at least enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);
        /* data must be non-null since we have at least one element */
        assert(list.data != NULL);
    } else {
        /* === STEP 2: Failure path === */
        /* length is unchanged */
        assert(list.length == old_list.length);
        /* data is unchanged */
        assert(list.data == old_list.data);
        /* current_size is unchanged */
        assert(list.current_size == old_list.current_size);
    }
}
