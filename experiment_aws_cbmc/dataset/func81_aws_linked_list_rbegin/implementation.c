aws_linked_list_rbegin(const struct aws_linked_list *list) {
    AWS_PRECONDITION(aws_linked_list_is_valid(list));
    struct aws_linked_list_node *rval = list->tail.prev;
    AWS_POSTCONDITION(aws_linked_list_is_valid(list));
    AWS_POSTCONDITION(rval == list->tail.prev);
    return rval;
}