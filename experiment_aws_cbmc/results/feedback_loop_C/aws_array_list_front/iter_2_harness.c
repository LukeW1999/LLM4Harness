#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    size_t initial_item_allocation = nondet_size_t() % max_initial_item_allocation;
    size_t item_size = 1 + (nondet_size_t() % max_item_size); // Ensure item_size is greater than 0
    struct aws_allocator *alloc = aws_default_allocator();
    uint8_t *raw_array = bounded_malloc(max_initial_item_allocation * max_item_size);
    size_t length = nondet_size_t() % max_initial_item_allocation;
    uint8_t val[item_size];

    // Initialize the list
    if (nondet_bool() && initial_item_allocation > 0) {
        aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);
    } else {
        __CPROVER_assume(raw_array != NULL);
        aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);
        list.length = length;
    }

    // Save old state
    struct aws_array_list old_list = list;
    uint8_t old_val[item_size];
    memcpy(old_val, val, item_size);

    // Ensure list is valid before calling aws_array_list_front
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(length > 0 && list.data != NULL);

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
}
