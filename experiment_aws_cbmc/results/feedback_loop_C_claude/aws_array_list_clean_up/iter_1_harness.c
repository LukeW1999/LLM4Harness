// === STEP 1: SUCCESS PATH ===
// aws_array_list_clean_up is void, no return value.
// After the call:
//   - list->alloc: CHANGES to NULL (AWS_ZERO_STRUCT zeroes everything)
//   - list->current_size: CHANGES to 0
//   - list->length: CHANGES to 0
//   - list->item_size: CHANGES to 0
//   - list->data: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always succeeds.
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (struct aws_array_list *):
//   - list->alloc: CHANGED to NULL always
//   - list->current_size: CHANGED to 0 always
//   - list->length: CHANGED to 0 always
//   - list->item_size: CHANGED to 0 always
//   - list->data: CHANGED to NULL always
//
// === STEP 4: VALIDITY INVARIANTS ===
// After clean_up, the list is zeroed out.
// aws_array_list_is_valid checks:
//   - (list->current_size == 0 || list->data != NULL) - satisfied since both are 0/NULL
//   - (list->length * list->item_size <= list->current_size) - 0*0 <= 0, satisfied
// So aws_array_list_is_valid(list) should hold after the call.

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_clean_up_harness(void) {
    /* Allocate and set up the array list */
    struct aws_array_list list;

    /* Use bounded list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the data member is allocated (or NULL) */
    ensure_array_list_has_allocated_data_member(&list);

    /* The list must be valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* === Verify frame conditions and post-conditions === */

    /* After AWS_ZERO_STRUCT(*list), all fields must be zero/NULL */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* The list should be valid after clean_up (zeroed state is valid) */
    assert(aws_array_list_is_valid(&list));
}
