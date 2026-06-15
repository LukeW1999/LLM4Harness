#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int result = aws_array_list_pop_back(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    } else {
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
