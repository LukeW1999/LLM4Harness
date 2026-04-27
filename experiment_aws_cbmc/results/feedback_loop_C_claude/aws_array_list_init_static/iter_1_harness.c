// === STEP 1: SUCCESS PATH ===
// aws_array_list_init_static is void - it always succeeds (or fatally aborts on precondition failure)
// On success:
//   - list->alloc: CHANGES to NULL
//   - list->current_size: CHANGES to item_count * item_size
//   - list->item_size: CHANGES to item_size
//   - list->length: CHANGES to 0
//   - list->data: CHANGES to raw_array
//
// === STEP 2: FAILURE PATH ===
// Function uses AWS_FATAL_PRECONDITION (aborts on failure), so no error return path.
// The function is void and always succeeds when preconditions are met.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list *):
//   - alloc: CHANGED to NULL
//   - current_size: CHANGED to item_count * item_size
//   - item_size: CHANGED to item_size parameter
//   - length: CHANGED to 0
//   - data: CHANGED to raw_array
// raw_array (void *): UNCHANGED (just stored as pointer)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)
//   - The list is static (alloc == NULL), data != NULL, current_size == item_count * item_size

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_array_list_init_static_harness() {
    /* Allocate the list struct */
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    /* Non-deterministic item_count and item_size, bounded to avoid overflow */
    size_t item_count;
    size_t item_size;
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    /* Ensure item_count * item_size does not overflow */
    size_t current_size_check;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size_check));

    /* Allocate raw_array with the correct size */
    void *raw_array = malloc(current_size_check);
    __CPROVER_assume(raw_array != NULL);

    /* Call the function under test */
    aws_array_list_init_static(list, raw_array, item_count, item_size);

    /* === STEP 1 assertions: verify all fields are set correctly === */
    assert(list->alloc == NULL);
    assert(list->current_size == current_size_check);
    assert(list->item_size == item_size);
    assert(list->length == 0);
    assert(list->data == raw_array);

    /* === STEP 4 assertions: validity invariant === */
    assert(aws_array_list_is_valid(list));
}
