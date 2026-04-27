aws_linked_list_rend(const struct aws_linked_list *list) {
    AWS_PRECONDITION(aws_linked_list_is_valid(list));
    const struct aws_linked_list_node *rval = &list->head;
    AWS_POSTCONDITION(aws_linked_list_is_valid(list));
    AWS_POSTCONDITION(rval == &list->head);
    return rval;
}