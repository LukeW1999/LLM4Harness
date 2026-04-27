aws_array_list_swap_contents(
    struct aws_array_list *AWS_RESTRICT list_a,
    struct aws_array_list *AWS_RESTRICT list_b) {
    AWS_FATAL_PRECONDITION(list_a->alloc);
    AWS_FATAL_PRECONDITION(list_a->alloc == list_b->alloc);
    AWS_FATAL_PRECONDITION(list_a->item_size == list_b->item_size);
    AWS_FATAL_PRECONDITION(list_a != list_b);
    AWS_PRECONDITION(aws_array_list_is_valid(list_a));
    AWS_PRECONDITION(aws_array_list_is_valid(list_b));

    struct aws_array_list tmp = *list_a;
    *list_a = *list_b;
    *list_b = tmp;
    AWS_POSTCONDITION(aws_array_list_is_valid(list_a));
    AWS_POSTCONDITION(aws_array_list_is_valid(list_b));
}