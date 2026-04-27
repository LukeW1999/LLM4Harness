// === STEP 1: SUCCESS PATH ===
// aws_array_list_clean_up always "succeeds" (void return).
// After the call, AWS_ZERO_STRUCT(*list) is always called, so:
//   - list->alloc: CHANGES to NULL (0)
//   - list->current_size: CHANGES to 0
//   - list->length: CHANGES to 0
//   - list->item_size: CHANGES to 0
//   - list->data: CHANGES to NULL (0)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always zeroes the struct.
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (struct aws_array_list *)
//   - list->alloc: CHANGED to NULL always
//   - list->current_size: CHANGED to 0 always
//   - list->length: CHANGED to 0 always
//   - list->item_size: CHANGED to 0 always
//   - list->data: CHANGED to NULL always
//
// === STEP 4: VALIDITY INVARIANTS ===
// After zeroing, aws_array_list_is_valid checks:
//   - list != NULL (yes, list is valid pointer)
//   - item_size != 0 OR (current_size == 0 AND data == NULL AND length == 0)
//   After zeroing: item_size=0, current_size=0, data=NULL, length=0
//   So the second condition holds: aws_array_list_is_valid should return true.
//   - aws_array_list_is_valid(list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_clean_up_harness(void) {
    /* Allocate a non-deterministic array list */
    struct aws_array_list list;

    /* Non-deterministically set up the list fields */
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();

    /* Non-deterministically decide if alloc and data are set */
    if (nondet_bool()) {
        /* Use a valid allocator */
        list.alloc = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        __CPROVER_assume(list.alloc != NULL);
    } else {
        list.alloc = NULL;
    }

    if (nondet_bool()) {
        /* Allocate some data */
        list.data = malloc(list.current_size > 0 ? list.current_size : 1);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* If alloc is set and data is set, we need a valid aws_mem_release path.
       For CBMC purposes, we just need the function to not crash.
       We assume the allocator is valid if non-null. */

    /* Save old state (not strictly needed since everything changes, but for clarity) */
    struct aws_array_list old_list = list;

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* === STEP 1 assertions: all fields zeroed === */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* === STEP 4: validity invariant === */
    /* After zeroing: item_size=0, current_size=0, data=NULL, length=0
       aws_array_list_is_valid should hold because the "empty static" case is valid */
    assert(aws_array_list_is_valid(&list));
}
