aws_array_list_init_static(
    struct aws_array_list *AWS_RESTRICT list,
    void *raw_array,
    size_t item_count,
    size_t item_size) {

    AWS_FATAL_PRECONDITION(list != NULL);
    AWS_FATAL_PRECONDITION(raw_array != NULL);
    AWS_FATAL_PRECONDITION(item_count > 0);
    AWS_FATAL_PRECONDITION(item_size > 0);

    AWS_ZERO_STRUCT(*list);
    list->alloc = NULL;

    size_t current_size = 0;
    int no_overflow = !aws_mul_size_checked(item_count, item_size, &current_size);
    AWS_FATAL_PRECONDITION(no_overflow);
    list->current_size = current_size;

    list->item_size = item_size;
    list->length = 0;
    list->data = raw_array;
    AWS_POSTCONDITION(aws_array_list_is_valid(list));
}