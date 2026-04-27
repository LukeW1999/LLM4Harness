aws_linked_list_next(const struct aws_linked_list_node *node) {
    struct aws_linked_list_node *rval = node->next;
    return rval;
}