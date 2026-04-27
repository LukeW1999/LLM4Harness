// === STEP 1: SUCCESS PATH ===
// When aws_array_list_init_dynamic returns AWS_OP_SUCCESS (or the successful value):
//   - list->alloc: CHANGES to alloc
//   - list->current_size: CHANGES to allocation_size
//   - list->length: CHANGES to 0
//   - list->item_size: CHANGES to item_size
//   - list->data: CHANGES to allocated memory

// === STEP 2: FAILURE PATH ===
// When aws_array_list_init_dynamic returns AWS_OP_ERR (or fails):
//   - list->alloc: UNCHANGED
//   - list->current_size: CHANGES to 0
//   - list->length: CHANGES to 0
//   - list->item_size: CHANGES to 0
//   - list->data: CHANGES to NULL

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - alloc: CHANGED on success, UNCHANGED on failure
//     - current_size: CHANGED on success, CHANGED on failure
//     - length: CHANGED always
//     - item_size: CHANGED on success, CHANGED on failure
//     - data: CHANGED on success, CHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_init_dynamic_harness() {
    struct aws_array_list list;
    struct aws_allocator alloc;
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    // Initialize the list to some arbitrary state
    AWS_ZERO_STRUCT(list);
    list.alloc = &alloc;
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();
    list.data = (void *)nondet_ptr();

    // Save old state
    struct aws_array_list old_list = list;

    int result = aws_array_list_init_dynamic(&list, &alloc, initial_item_allocation, item_size);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == &alloc);
        assert(list.current_size == (initial_item_allocation * item_size) || list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == item_size);
        assert(list.data != NULL || initial_item_allocation == 0);
        assert(aws_array_list_is_valid(&list));
    }
    // Assertions for failure path
    else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.data == NULL);
        assert(aws_array_list_is_valid(&list));
    }
}
