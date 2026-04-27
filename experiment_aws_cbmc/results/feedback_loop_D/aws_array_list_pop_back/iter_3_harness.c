#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    struct aws_allocator *allocator = aws_default_allocator();
    ensure_array_list_has_allocated_data_member(&list, allocator);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_item_alloc, max_item_size));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    // Ensure the list has at least one item to pop
    __CPROVER_assume(list.length > 0);

    struct aws_array_list old_list = list;
    int result = aws_array_list_pop_back(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length - 1);
        assert(list.data == old_list.data);
        assert(list.item_size == old_list.item_size);
        assert(list.current_size == old_list.current_size);
        assert(list.alloc == old_list.alloc);
    } else {
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
        assert(list.item_size == old_list.item_size);
        assert(list.current_size == old_list.current_size);
        assert(list.alloc == old_list.alloc);
    }

    assert(aws_array_list_is_valid(&list));
}
