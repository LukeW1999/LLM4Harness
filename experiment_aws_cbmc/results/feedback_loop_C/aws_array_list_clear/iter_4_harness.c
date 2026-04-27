#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list;
    size_t initial_length;
    void *initial_data;

    // Initialize and assume preconditions
    __CPROVER_assume(aws_array_list_init_dynamic(&list, 10, sizeof(int)) == AWS_OP_SUCCESS);
    old_list = list;
    initial_length = list.length;
    initial_data = list.data;

    // Assume additional necessary conditions
    __CPROVER_assume(aws_array_list_is_valid(&list));

    int result = aws_array_list_clear(&list);

    // Check postconditions for success
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == 0);
        assert(list.data == initial_data); // Assuming data pointer does not change on clear
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
    } 
    // Check postconditions for failure
    else {
        assert(list.length == initial_length);
        assert(list.data == initial_data);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
}
