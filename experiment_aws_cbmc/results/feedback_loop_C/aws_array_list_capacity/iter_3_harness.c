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
    struct aws_allocator *alloc = can_fail_allocator();
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
    if (list.alloc == NULL && old_list.data != NULL) {
        free(old_list.data);
    }
}
