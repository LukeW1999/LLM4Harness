aws_linked_list_rbegin(const struct aws_linked_list *list) {
    struct aws_linked_list_node *rval = list->tail.prev;
    return rval;
}