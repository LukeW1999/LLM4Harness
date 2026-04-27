aws_linked_list_prev(const struct aws_linked_list_node *node) {
    struct aws_linked_list_node *rval = node->prev;
    return rval;
}