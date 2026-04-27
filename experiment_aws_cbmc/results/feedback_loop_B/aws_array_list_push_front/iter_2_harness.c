#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;
    size_t max_initial_item_allocation = nd_size_t();
    size_t max_item_size = nd_size_t();
    __CPROVER_assume(max_initial_item_allocation > 0 && max_initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(max_item_size > 0 && max_item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_initial_item_allocation, max_item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old_list = list;
    uint8_t val[max_item_size];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, max_item_size));

    int result = aws_array_list_push_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length + 1);
        assert(memcmp(list.data, val, max_item_size) == 0);
        if (old_list.length > 0) {
            assert(memcmp((uint8_t *)list.data + max_item_size, old_list.data, old_list.length * max_item_size) == 0);
        }
    } else {
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
    }

    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);
    assert(list.current_size == old_list.current_size || list.current_size > old_list.current_size);

    assert(aws_array_list_is_valid(&list));
}
