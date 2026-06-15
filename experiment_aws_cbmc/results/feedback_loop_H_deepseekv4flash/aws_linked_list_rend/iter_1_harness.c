#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    const struct aws_linked_list_node *rval = aws_linked_list_rend(&list);

    assert(rval == &list.head);
    assert(aws_linked_list_is_valid_deep(&list));
}
