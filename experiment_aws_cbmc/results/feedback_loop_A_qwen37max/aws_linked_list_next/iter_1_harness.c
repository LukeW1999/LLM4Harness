#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *node = list.head.next;
    struct aws_linked_list_node *old_next = node->next;

    struct aws_linked_list_node *result = aws_linked_list_next(node);

    assert(result == old_next);
    assert(aws_linked_list_is_valid(&list));
}
