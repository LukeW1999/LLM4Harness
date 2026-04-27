#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    void *val = malloc(sizeof(uint8_t) * 10); // Assuming item_size <= 10 for simplicity
    size_t index = nondet_size_t();

    // Initialize list with some values
    aws_array_list_init_dynamic(&list, NULL, 10, 10); // initial_item_allocation and item_size set to 10
    ensure_array_list_has_allocated_data_member(&list);

    // Set initial values in the list for testing
    for (size_t i = 0; i < list.current_size; i++) {
        uint8_t initial_value = (uint8_t)(i % 256);
        aws_array_list_set_at(&list, &initial_value, i);
    }

    // Save old state
    struct aws_array_list old_list = list;

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
