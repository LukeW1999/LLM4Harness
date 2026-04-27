aws_linked_list_next(const struct aws_linked_list_node *node) {
    AWS_PRECONDITION(aws_linked_list_node_next_is_valid(node));
    struct aws_linked_list_node *rval = node->next;
    AWS_POSTCONDITION(aws_linked_list_node_next_is_valid(node));
    AWS_POSTCONDITION(aws_linked_list_node_prev_is_valid(rval));
    AWS_POSTCONDITION(rval == node->next);
    return rval;
}