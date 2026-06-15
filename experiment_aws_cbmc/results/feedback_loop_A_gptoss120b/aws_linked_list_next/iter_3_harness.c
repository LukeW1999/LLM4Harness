#include <assert.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_next_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure the list has at least one real node (not the tail sentinel) */
    __CPROVER_assume(list.head.next != &list.tail);
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(node != NULL);

    /* Save copies of the original state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* Post‑conditions */
    assert(result == old_node.next);
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
