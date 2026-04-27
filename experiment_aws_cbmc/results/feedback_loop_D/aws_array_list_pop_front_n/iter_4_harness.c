#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_array_list list;
    size_t n = nondet_size_t();

    // Initialize the array list with some arbitrary values
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    struct aws_allocator *alloc = can_fail_allocator();
    __CPROVER_assume(alloc != NULL);

    // Initialize the list
    if (aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size) == AWS_OP_SUCCESS) {
        // Ensure the list has allocated data
        ensure_array_list_has_allocated_data_member(&list);

        // Populate the list with some items
        size_t initial_length = nondet_size_t();
        __CPROVER_assume(initial_length <= initial_item_allocation);
        for (size_t i = 0; i < initial_length; i++) {
            void *item = malloc(item_size);
            __CPROVER_assume(item != NULL);
            aws_array_list_push_back(&list, item);
        }

        // Save old state
        struct aws_array_list old_list = list;

        // Call the function under test
        int result = aws_array_list_pop_front_n(&list, n);

        // Assertions based on the analysis
        if (result == AWS_OP_SUCCESS) {
            if (n >= aws_array_list_length(&old_list)) {
                assert(list.length == 0);
                assert(list.data == NULL || list.current_size == 0); // Assuming data is NULL when length is 0
            } else if (n > 0) {
                size_t remaining_items = aws_array_list_length(&old_list) - n;
                assert(list.length == remaining_items);
                assert(list.item_size == old_list.item_size);
                assert(list.alloc == old_list.alloc);
                assert(list.data != NULL || list.current_size == 0); // Assuming data is NULL when length is 0
            } else {
                assert(list.length == old_list.length);
                assert(list.current_size == old_list.current_size);
                assert(list.item_size == old_list.item_size);
                assert(list.data == old_list.data);
                assert(list.alloc == old_list.alloc);
            }
        } else { // AWS_OP_ERR
            assert(list.length == old_list.length);
            assert(list.current_size == old_list.current_size);
            assert(list.item_size == old_list.item_size);
            assert(list.data == old_list.data);
            assert(list.alloc == old_list.alloc);
        }

        // Validity invariant
        assert(aws_array_list_is_valid(&list));

        // Clean up
        aws_array_list_clean_up(&list);
    }

    aws_mem_release(alloc, alloc);
}
