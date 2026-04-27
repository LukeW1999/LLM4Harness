aws_linked_list_remove(struct aws_linked_list_node *node) {
    AWS_PRECONDITION(aws_linked_list_node_prev_is_valid(node));
    AWS_PRECONDITION(aws_linked_list_node_next_is_valid(node));
    node->prev->next = node->next;
    node->next->prev = node->prev;
    aws_linked_list_node_reset(node);
    AWS_POSTCONDITION(node->next == NULL && node->prev == NULL);
}