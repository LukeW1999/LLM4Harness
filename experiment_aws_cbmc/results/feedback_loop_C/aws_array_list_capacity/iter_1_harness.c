// === STEP 1: SUCCESS PATH ===
// When aws_array_list_ensure_capacity returns AWS_OP_SUCCESS (or the successful value):
//   - list->current_size: CHANGES to a new value that is sufficient to accommodate the index
//   - list->data: CHANGES to a new allocated memory block if necessary
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_ensure_capacity returns AWS_OP_ERR (or fails):
//   - list->current_size: UNCHANGED
//   - list->data: UNCHANGED
//   - list->length: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - data: CHANGED on success, UNCHANGED on failure
//     - length: UNCHANGED always
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_capacity_harness() {
    struct aws_array_list list;
    size_t index = nondet_size_t();

    // Initialize list with some arbitrary values
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t() + 1; // Ensure item_size is greater than 0
    struct aws_allocator *alloc = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(alloc != NULL);

    // Initialize the list dynamically
    if (nondet_bool()) {
        aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);
    } else {
        // Initialize the list statically
        size_t item_count = nondet_size_t() + 1; // Ensure item_count is greater than 0
        void *raw_array = malloc(item_count * item_size);
        __CPROVER_assume(raw_array != NULL);
        aws_array_list_init_static(&list, raw_array, item_count, item_size);
    }

    // Save old state
    struct aws_array_list old_list = list;

    // Call the function under test
    int result = aws_array_list_ensure_capacity(&list, index);

    // Step 1: Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(list.current_size >= (index + 1) * list.item_size);
        assert(list.data != NULL);
    }

    // Step 2: Failure path assertions
    if (result == AWS_OP_ERR) {
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }

    // Step 3: Frame conditions
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    // Step 4: Validity invariants
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
    free(alloc);
    if (list.alloc == NULL) {
        free(list.data);
    }
}
