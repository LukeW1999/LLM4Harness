#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(initial_item_allocation <= max_initial_item_allocation);
    __CPROVER_assume(item_size <= max_item_size);

    aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    old_list = list;

    int result = aws_array_list_pop_front(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
        assert(list.length == old_list.length - 1);
        assert(list.current_size == old_list.current_size - old_list.item_size);
        // We cannot assert exact data contents due to shifting, but we can assert the size and length constraints
    } else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    assert(aws_array_list_is_valid(&list));
    aws_array_list_clean_up(&list);
}
