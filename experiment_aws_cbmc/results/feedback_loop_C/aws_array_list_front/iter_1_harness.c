// === STEP 1: SUCCESS PATH ===
// When aws_array_list_front returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.data: UNCHANGED
//   - val: CHANGES to the value of the first element in the list
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_front returns AWS_OP_ERR (or fails):
//   - list.length: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.data: UNCHANGED
//   - val: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - length: UNCHANGED always
//     - current_size: UNCHANGED always
//     - item_size: UNCHANGED always
//     - data: UNCHANGED always
//     - alloc: UNCHANGED always
//   val (void*):
//     - CHANGES on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)
//   - val: NO (not a struct, just a pointer to memory)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    size_t initial_item_allocation = nondet_size_t() % max_initial_item_allocation;
    size_t item_size = 1 + (nondet_size_t() % max_item_size); // Ensure item_size is greater than 0
    struct aws_allocator *alloc = can_fail_allocator_new();
    __CPROVER_assume(alloc != NULL);
    uint8_t *raw_array = bounded_malloc(max_initial_item_allocation * max_item_size);
    size_t length = nondet_size_t() % max_initial_item_allocation;
    size_t index = nondet_size_t() % max_initial_item_allocation;
    uint8_t val[item_size];

    // Initialize the list
    if (nondet_bool()) {
        aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);
    } else {
        aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);
        list.length = length;
    }

    // Save old state
    struct aws_array_list old_list = list;
    uint8_t old_val[item_size];
    memcpy(old_val, val, item_size);

    // Call the function
    int result = aws_array_list_front(&list, val);

    // Assertions based on the analysis
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        assert(memcmp(val, old_list.data, item_size) == 0);
    } else {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        assert(memcmp(val, old_val, item_size) == 0);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
    can_fail_allocator_free(alloc);
}
