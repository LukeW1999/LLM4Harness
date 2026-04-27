// === STEP 1: SUCCESS PATH ===
// When aws_array_list_capacity returns AWS_OP_SUCCESS (or the successful value):
//   - list.current_size: CHANGES to a value that can accommodate at least (index + 1) * list.item_size
//   - list.length: UNCHANGED unless index >= list.length, in which case it may change to index + 1
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_capacity returns AWS_OP_ERR (or fails):
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.data: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success if index >= list.length, UNCHANGED otherwise
//     - data: UNCHANGED always
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness() {
    struct aws_array_list list;
    size_t index = nondet_size_t();
    struct aws_array_list old_list = list;

    // Initialize the list with some arbitrary values
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    assume(item_size > 0);
    struct aws_allocator *alloc = malloc(sizeof(struct aws_allocator));
    assume(alloc != NULL);
    aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    // Ensure the list is valid before the call
    assert(aws_array_list_is_valid(&list));

    // Save old state
    struct aws_array_list old = list;

    int result = aws_array_list_capacity(&list);

    // Check frame conditions and postconditions based on the result
    if (result == AWS_OP_SUCCESS) {
        assert(list.current_size >= (index + 1) * list.item_size);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    // Check validity invariant
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
    free(alloc);
}
