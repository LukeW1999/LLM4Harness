aws_linked_list_end(const struct aws_linked_list *list) {
    AWS_PRECONDITION(aws_linked_list_is_valid(list));
    const struct aws_linked_list_node *rval = &list->tail;
    AWS_POSTCONDITION(aws_linked_list_is_valid(list));
    AWS_POSTCONDITION(rval == &list->tail);
    return rval;
}