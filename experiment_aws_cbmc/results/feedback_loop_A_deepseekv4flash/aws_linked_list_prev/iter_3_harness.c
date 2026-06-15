#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != &list.tail);
    /* Ensure node is a valid interior node (prev and next non-NULL) */
    __CPROVER_assume(node->prev != NULL && node->next != NULL);

    struct aws_linked_list_node *expected_prev = node->prev;

    struct aws_linked_list_node *rval = aws_linked_list_prev(node);

    assert(rval == expected_prev);
    assert(aws_linked_list_is_valid(&list));
    assert(node->prev == expected_prev);
    assert(node->next != NULL);
    assert(rval != NULL);
}
