#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 256

void aws_array_list_clear_harness() {
    struct aws_array_list list;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int rval = aws_array_list_clear(&list);

    assert(aws_array_list_is_valid(&list));
    assert(rval == AWS_OP_SUCCESS);
    assert(list.length == 0);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.alloc == old.alloc);
}
