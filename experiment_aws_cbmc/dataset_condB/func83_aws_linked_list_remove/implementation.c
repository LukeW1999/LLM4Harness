aws_linked_list_remove(struct aws_linked_list_node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    aws_linked_list_node_reset(node);
}