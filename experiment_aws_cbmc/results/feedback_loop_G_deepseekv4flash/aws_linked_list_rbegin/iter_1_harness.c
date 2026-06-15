#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);
    assert(rval == list.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}
