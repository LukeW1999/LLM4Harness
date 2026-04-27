// === STEP 1: SUCCESS PATH ===
// aws_array_list_clear returns void (no return value)
// When list->data != NULL:
//   - list->length: CHANGES to 0
//   - list->data: UNCHANGED (pointer itself unchanged)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
// When list->data == NULL:
//   - list->length: UNCHANGED (no modification)
//   - list->data: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// Function is void, no failure path.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - list->length: CHANGED to 0 if list->data != NULL, UNCHANGED otherwise
//   - list->data: UNCHANGED always (pointer value)
//   - list->current_size: UNCHANGED always
//   - list->item_size: UNCHANGED always
//   - list->alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (postcondition states AWS_IS_ZEROED(*list) || aws_array_list_is_valid(list))

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_clear_harness(void) {
    // Allocate and initialize a valid aws_array_list
    struct aws_array_list list;

    // Use bounded list to keep verification tractable
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    // Assume the list is valid (or zeroed, per precondition)
    __CPROVER_assume(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    // Save old state
    struct aws_array_list old_list = list;

    // Call the function under test
    aws_array_list_clear(&list);

    // === Frame condition assertions ===

    // list->data pointer itself is unchanged
    assert(list.data == old_list.data);

    // list->current_size is unchanged
    assert(list.current_size == old_list.current_size);

    // list->item_size is unchanged
    assert(list.item_size == old_list.item_size);

    // list->alloc is unchanged
    assert(list.alloc == old_list.alloc);

    // === Functional assertions ===

    // If data was non-NULL, length should be 0
    if (old_list.data != NULL) {
        assert(list.length == 0);
    } else {
        // If data was NULL, length is unchanged
        assert(list.length == old_list.length);
    }

    // === Validity invariant ===
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));
}
