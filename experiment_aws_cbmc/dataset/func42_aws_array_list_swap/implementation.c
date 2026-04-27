aws_array_list_swap(struct aws_array_list *AWS_RESTRICT list, size_t a, size_t b) {
    AWS_FATAL_PRECONDITION(a < list->length);
    AWS_FATAL_PRECONDITION(b < list->length);
    AWS_PRECONDITION(aws_array_list_is_valid(list));

    if (a == b) {
        AWS_POSTCONDITION(aws_array_list_is_valid(list));
        return;
    }

    void *item1 = NULL;
    void *item2 = NULL;
    aws_array_list_get_at_ptr(list, &item1, a);
    aws_array_list_get_at_ptr(list, &item2, b);
    aws_array_list_mem_swap(item1, item2, list->item_size);
    AWS_POSTCONDITION(aws_array_list_is_valid(list));
}