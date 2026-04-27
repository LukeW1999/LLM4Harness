#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_length_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE) == AWS_OP_SUCCESS);

    // Capture the old state of the list
    struct aws_array_list old_list = list;

    // Call the function under test
    size_t result = aws_array_list_length(&list);

    // Assertions based on the analysis
    assert(aws_array_list_is_valid(&list));

    if (result != AWS_OP_ERR) {
        // Success path assertions
        assert(list.length == result);
    } else {
        // Failure path assertions
        assert(list.alloc == old_list.alloc);
        assert(list.data == old_list.data);
        assert(list.item_size == old_list.item_size);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
    }

    // Clean up
    aws_array_list_clean_up(&list);
}
