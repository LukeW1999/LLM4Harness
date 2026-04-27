#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_length_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    // Initialize the list with valid parameters
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_init(&list, allocator, initial_item_allocation, item_size) == AWS_OP_SUCCESS);

    // Capture the old state of the list
    struct aws_array_list old_list = list;

    // Call the function under test
    size_t result = aws_array_list_length(&list);

    // Assertions based on the analysis
    assert(aws_array_list_is_valid(&list));

    // Success path assertions
    assert(list.length == result);

    // Clean up
    aws_array_list_clean_up(&list);
}
