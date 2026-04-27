// === STEP 1: SUCCESS PATH ===
// aws_array_list_init_static does not return a value (void function).
// It uses AWS_FATAL_PRECONDITION which aborts on failure, so if we reach
// the end of the function, all preconditions were satisfied.
// On success:
//   - list->alloc: CHANGES to NULL
//   - list->current_size: CHANGES to item_count * item_size (no overflow)
//   - list->item_size: CHANGES to item_size
//   - list->length: CHANGES to 0
//   - list->data: CHANGES to raw_array
//
// === STEP 2: FAILURE PATH ===
// The function uses AWS_FATAL_PRECONDITION (aborts on failure), so there is
// no "failure path" that returns - it either succeeds or aborts.
// We only need to verify the success path.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list *):
//   - alloc: CHANGED to NULL
//   - current_size: CHANGED to item_count * item_size
//   - item_size: CHANGED to item_size parameter
//   - length: CHANGED to 0
//   - data: CHANGED to raw_array
// raw_array (void *): UNCHANGED (just stored as pointer)
// item_count (size_t): UNCHANGED (input parameter)
// item_size (size_t): UNCHANGED (input parameter)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)
//     aws_array_list_is_valid checks:
//       - list != NULL
//       - (list->current_size == 0 && list->data == NULL) ||
//         (list->current_size != 0 && list->data != NULL && list->item_size != 0)
//       - list->length * list->item_size <= list->current_size

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_init_static_harness() {
    // Allocate the list struct
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    // Nondeterministic item_count and item_size, both > 0 (preconditions)
    size_t item_count;
    size_t item_size;
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    // Ensure no overflow: item_count * item_size must not overflow
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    // Allocate raw_array with sufficient size
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    // Call the function under test
    aws_array_list_init_static(list, raw_array, item_count, item_size);

    // === STEP 1 assertions: verify post-conditions ===
    assert(list->alloc == NULL);
    assert(list->current_size == current_size);
    assert(list->item_size == item_size);
    assert(list->length == 0);
    assert(list->data == raw_array);

    // === STEP 4 assertions: validity invariant ===
    assert(aws_array_list_is_valid(list));
}
