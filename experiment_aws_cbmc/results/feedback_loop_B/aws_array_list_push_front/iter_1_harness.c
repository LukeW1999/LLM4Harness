#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old_list = list;
    uint8_t val[list.item_size];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    int result = aws_array_list_push_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length + 1);
        assert(memcmp(list.data, val, list.item_size) == 0);
        if (old_list.length > 0) {
            assert(memcmp((uint8_t *)list.data + list.item_size, old_list.data, old_list.length * list.item_size) == 0);
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
