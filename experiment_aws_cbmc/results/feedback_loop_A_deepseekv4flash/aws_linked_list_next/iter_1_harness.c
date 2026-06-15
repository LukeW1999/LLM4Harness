#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    /* Create a list and a node */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Initialize list */
    aws_linked_list_init(&list);

    /* Insert node into list (e.g., push_back) */
    aws_linked_list_push_back(&list, &node);

    /* Assume list is valid */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* Save old node->next and node->prev */
    struct aws_linked_list_node *old_next = node.next;
    struct aws_linked_list_node *old_prev = node.prev;

    /* Call function */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* Assert return value equals node->next */
    assert(result == node.nextapse);
    /* Assert node fields unchanged */
    assert(node.next == old_next);
    assert(node.prev == old_prev);
    /* Assert list still valid */
    assert(aws_linked_list_is_valid_deep(&list));
}
