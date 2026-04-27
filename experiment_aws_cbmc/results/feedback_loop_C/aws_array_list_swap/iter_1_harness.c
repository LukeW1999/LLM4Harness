// === STEP 1: SUCCESS PATH ===
// When aws_array_list_swap returns AWS_OP_SUCCESS (or the successful value):
//   - list.data[a]: CHANGES to list.data[b]'s original value
//   - list.data[b]: CHANGES to list.data[a]'s original value
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_swap returns AWS_OP_ERR (or fails):
//   - list.data[a]: UNCHANGED
//   - list.data[b]: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.alloc: UNCHANGED
//   - list.data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - current_size: UNCHANGED always
//     - length: UNCHANGED always
//     - item_size: UNCHANGED always
//     - data: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    // Initialize the list with some arbitrary values
    struct aws_allocator *alloc = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(alloc != NULL);
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation > 0);
    __CPROVER_assume(aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size) == AWS_OP_SUCCESS);

    // Ensure a and b are valid indices
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    // Save old state
    struct aws_array_list old_list = list;
    void *old_data_a = list.data + a * list.item_size;
    void *old_data_b = list.data + b * list.item_size;

    // Call the function under test
    aws_array_list_swap(&list, a, b);

    // Assert frame conditions and validity invariants
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);

    // Check that the swap occurred correctly
    if (a != b) {
        assert_bytes_match((uint8_t *)list.data + a * list.item_size, (uint8_t *)old_data_b, list.item_size);
        assert_bytes_match((uint8_t *)list.data + b * list.item_size, (uint8_t *)old_data_a, list.item_size);
    } else {
        assert(list.data == old_list.data);
    }

    // Check that the list remains valid
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
    free(alloc);
}
