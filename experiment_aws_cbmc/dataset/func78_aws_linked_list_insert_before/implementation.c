aws_linked_list_insert_before(
    struct aws_linked_list_node *before,
    struct aws_linked_list_node *to_add) {
    AWS_PRECONDITION(aws_linked_list_node_prev_is_valid(before));
    AWS_PRECONDITION(to_add != NULL);
    to_add->next = before;
    to_add->prev = before->prev;
    before->prev->next = to_add;
    before->prev = to_add;
    AWS_POSTCONDITION(aws_linked_list_node_prev_is_valid(before));
    AWS_POSTCONDITION(aws_linked_list_node_prev_is_valid(to_add));
    AWS_POSTCONDITION(aws_linked_list_node_next_is_valid(to_add));
    AWS_POSTCONDITION(before->prev == to_add);
}