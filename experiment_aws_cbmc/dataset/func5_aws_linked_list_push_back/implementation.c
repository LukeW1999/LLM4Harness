/* From: include/aws/common/linked_list.inl */

/* Helper: insert node before next_node in the list */
AWS_STATIC_IMPL void aws_linked_list_insert_before(
    struct aws_linked_list_node *next_node,
    struct aws_linked_list_node *node) {
    AWS_PRECONDITION(aws_linked_list_node_prev_is_valid(next_node));
    node->next = next_node;
    node->prev = next_node->prev;
    node->prev->next = node;
    next_node->prev = node;
    AWS_POSTCONDITION(aws_linked_list_node_prev_is_valid(next_node));
    AWS_POSTCONDITION(aws_linked_list_node_next_is_valid(node));
    AWS_POSTCONDITION(aws_linked_list_node_prev_is_valid(node));
}

/**
 * Append new_node to the back of the list (just before tail sentinel).
 */
AWS_STATIC_IMPL void aws_linked_list_push_back(
    struct aws_linked_list *list,
    struct aws_linked_list_node *node) {
    AWS_PRECONDITION(aws_linked_list_is_valid(list));
    AWS_PRECONDITION(node != NULL);
    aws_linked_list_insert_before(&list->tail, node);
    AWS_POSTCONDITION(aws_linked_list_is_valid(list));
    AWS_POSTCONDITION(list->tail.prev == node, "[node] is the new last element of [list]");
}
