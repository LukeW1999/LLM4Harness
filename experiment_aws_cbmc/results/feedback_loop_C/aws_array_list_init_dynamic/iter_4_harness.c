#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_init_dynamic_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    // Initialize the list to some arbitrary state
    AWS_ZERO_STRUCT(list);
    list.alloc = allocator;
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();
    list.data = (void *)nondet_ptr();

    // Save old state
    struct aws_array_list old_list = list;

    int result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == allocator);
        assert(list.current_size == (initial_item_allocation * item_size) || list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == item_size);
        assert(list.data != NULL || initial_item_allocation == 0);
        assert(aws_array_list_is_valid(&list));
    }
    // Assertions for failure path
    else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.data == NULL);
        assert(aws_array_list_is_valid(&list));
    }
}
