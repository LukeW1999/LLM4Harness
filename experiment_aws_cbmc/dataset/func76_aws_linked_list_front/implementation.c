aws_linked_list_front(const struct aws_linked_list *list) {
    AWS_PRECONDITION(aws_linked_list_is_valid(list));
    AWS_PRECONDITION(!aws_linked_list_empty(list));
    struct aws_linked_list_node *rval = list->head.next;
    AWS_POSTCONDITION(aws_linked_list_is_valid(list));
    AWS_POSTCONDITION(aws_linked_list_node_prev_is_valid(rval));
    AWS_POSTCONDITION(aws_linked_list_node_next_is_valid(rval));
    return rval;
}