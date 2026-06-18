#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *node = &list.tail;
    struct aws_linked_list_node *old_prev = node->prev;

    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    assert(result == old_prev);
    assert(aws_linked_list_is_valid(&list));
}
