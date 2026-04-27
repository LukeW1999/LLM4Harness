aws_linked_list_insert_after(
    struct aws_linked_list_node *after,
    struct aws_linked_list_node *to_add) {
    AWS_PRECONDITION(aws_linked_list_node_next_is_valid(after));
    AWS_PRECONDITION(to_add != NULL);
    to_add->prev = after;
    to_add->next = after->next;
    after->next->prev = to_add;
    after->next = to_add;
    AWS_POSTCONDITION(aws_linked_list_node_next_is_valid(after));
    AWS_POSTCONDITION(aws_linked_list_node_prev_is_valid(to_add));
    AWS_POSTCONDITION(aws_linked_list_node_next_is_valid(to_add));
    AWS_POSTCONDITION(after->next == to_add);
}