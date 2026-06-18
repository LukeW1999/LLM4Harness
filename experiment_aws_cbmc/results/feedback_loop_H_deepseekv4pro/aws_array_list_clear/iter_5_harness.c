#include <aws/common/array_list.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

#define MAX_INITIAL_ITEM_ALLOCATION 2
#define MAX_ITEM_SIZE 256

void aws_array_list_clear_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    aws_array_list_clear(&list);

    assert(aws_array_list_is_valid(&list));
    assert(list.length == 0);
    assert(list.data == old.data);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.alloc == old.alloc);
}
