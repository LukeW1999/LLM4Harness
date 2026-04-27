#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list;
    size_t index;
    uint8_t val[list.item_size];

    // Initialize the list with some arbitrary values
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    __CPROVER_assume(max_item_size > 0 && max_item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(max_item_alloc > 0 && max_item_alloc <= MAX_INITIAL_ITEM_ALLOCATION);
    aws_array_list_init_static(&list, malloc(max_item_alloc * max_item_size), max_item_alloc, max_item_size);
    ensure_array_list_has_allocated_data_member(&list);

    // Save the old state of the list
    old_list = list;

    // Non-deterministically choose an index and a value
    index = nondet_size_t();
    __CPROVER_assume(index <= max_item_alloc); // index should be within the bounds of the allocated array

    // Initialize val with non-deterministic values
    for (size_t i = 0; i < max_item_size; i++) {
        val[i] = nondet_uint8_t();
    }

    // Call the function under test
    int result = aws_array_list_set_at(&list, val, index);

    // Assert the frame conditions and validity invariants
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);

    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }
        // Check if the data at the specified index is correctly set
        assert(memcmp((void *)((uint8_t *)list.data + (list.item_size * index)), val, list.item_size) == 0);
    } else {
        // Failure path assertions
        assert(list.length == old_list.length);
        assert(memcmp(list.data, old_list.data, old_list.current_size) == 0);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
}
