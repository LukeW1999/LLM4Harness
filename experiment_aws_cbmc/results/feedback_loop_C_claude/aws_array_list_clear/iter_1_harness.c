// === STEP 1: SUCCESS PATH ===
// aws_array_list_clear returns void (no return value)
// When list->data is non-NULL:
//   - list->length: CHANGES to 0
//   - list->data: UNCHANGED (pointer itself unchanged)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
// When list->data is NULL (zeroed struct):
//   - list->length: UNCHANGED (already 0)
//   - list->data: UNCHANGED (NULL)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// aws_array_list_clear is void, no failure path
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - length: CHANGED to 0 if data != NULL, UNCHANGED if data == NULL
//   - data: UNCHANGED always (pointer value)
//   - current_size: UNCHANGED always
//   - item_size: UNCHANGED always
//   - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (postcondition states AWS_IS_ZEROED(*list) || aws_array_list_is_valid(list))

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_array_list_clear_harness() {
    /* Declare and initialize the array list */
    struct aws_array_list list;

    /* Use bounded list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data member */
    ensure_array_list_has_allocated_data_member(&list);

    /* The precondition: AWS_IS_ZEROED(*list) || aws_array_list_is_valid(list) */
    __CPROVER_assume(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* Call the function under test */
    aws_array_list_clear(&list);

    /* === FRAME CONDITIONS === */
    /* data pointer is unchanged */
    assert(list.data == old.data);
    /* current_size is unchanged */
    assert(list.current_size == old.current_size);
    /* item_size is unchanged */
    assert(list.item_size == old.item_size);
    /* alloc is unchanged */
    assert(list.alloc == old.alloc);

    /* === SUCCESS PATH === */
    /* If data was non-NULL, length is set to 0 */
    if (old.data != NULL) {
        assert(list.length == 0);
    }
    /* If data was NULL (zeroed), length remains unchanged (0) */
    if (old.data == NULL) {
        assert(list.length == old.length);
    }

    /* === VALIDITY INVARIANTS === */
    /* Postcondition: AWS_IS_ZEROED(*list) || aws_array_list_is_valid(list) */
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));
}
