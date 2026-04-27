aws_linked_list_end(const struct aws_linked_list *list) {
    const struct aws_linked_list_node *rval = &list->tail;
    return rval;
}