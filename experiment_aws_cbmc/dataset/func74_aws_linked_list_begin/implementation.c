aws_linked_list_begin(const struct aws_linked_list *list) {
    AWS_PRECONDITION(aws_linked_list_is_valid(list));
    struct aws_linked_list_node *rval = list->head.next;
    AWS_POSTCONDITION(aws_linked_list_is_valid(list));
    AWS_POSTCONDITION(rval == list->head.next);
    return rval;
}