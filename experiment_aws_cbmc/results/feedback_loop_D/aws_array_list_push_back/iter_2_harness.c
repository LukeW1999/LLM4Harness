#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_back_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    uint8_t val[item_size];

    aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);

    struct aws_array_list old_list = list;
    int result = aws_array_list_push_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length + 1);
        if (old_list.current_size < list.length * list.item_size) {
            assert(list.data != old_list.data);
            assert(list.current_size >= list.length * list.item_size);
        } else {
            assert(list.data == old_list.data);
            assert(list.current_size == old_list.current_size);
        }
    } else {
        assert(list.data == old_list.data);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
    }

    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
