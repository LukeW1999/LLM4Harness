// === STEP 1: SUCCESS PATH ===
// When aws_array_list_set_at returns AWS_OP_SUCCESS (or the successful value):
//   - list.data: CHANGES to have new value at index * list.item_size
//   - list.length: CHANGES to index + 1 if index >= list.length

// === STEP 2: FAILURE PATH ===
// When aws_array_list_set_at returns AWS_OP_ERR (or fails):
//   - list.alloc: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.data: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// list (struct type):
//   - alloc: UNCHANGED always
//   - current_size: UNCHANGED always
//   - length: CHANGED on success if index >= list.length, UNCHANGED on failure
//   - item_size: UNCHANGED always
//   - data: CHANGED on success to have new value at index * list.item_size, UNCHANGED on failure

// val (void type, not a struct):
//   - val: UNCHANGED always

// index (size_t type, not a struct):
//   - index: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list;
    void *val = malloc(sizeof(uint8_t) * 10); // Assuming item_size <= 10 for simplicity
    size_t index = nondet_size_t();

    // Initialize list with some values
    aws_array_list_init_dynamic(&list, NULL, 10, 10); // initial_item_allocation and item_size set to 10
    ensure_array_list_has_allocated_data_member(&list);

    // Save old state
    old_list = list;

    // Call the function under test
    int result = aws_array_list_set_at(&list, val, index);

    // Assertions based on analysis
    if (result == AWS_OP_SUCCESS) {
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }
        uint8_t expected_value[10];
        memcpy(expected_value, val, list.item_size);
        uint8_t actual_value[10];
        memcpy(actual_value, (void *)((uint8_t *)list.data + (list.item_size * index)), list.item_size);
        assert_bytes_match(expected_value, actual_value, list.item_size);
    } else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
    free(val);
}
