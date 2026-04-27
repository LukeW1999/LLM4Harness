aws_array_list_pop_front_n(struct aws_array_list *AWS_RESTRICT list, size_t n) {
    AWS_PRECONDITION(aws_array_list_is_valid(list));
    if (n >= aws_array_list_length(list)) {
        aws_array_list_clear(list);
        AWS_POSTCONDITION(aws_array_list_is_valid(list));
        return;
    }

    if (n > 0) {
        size_t popping_bytes = list->item_size * n;
        size_t remaining_items = aws_array_list_length(list) - n;
        size_t remaining_bytes = remaining_items * list->item_size;
        memmove(list->data, (uint8_t *)list->data + popping_bytes, remaining_bytes);
        list->length = remaining_items;
#ifdef DEBUG_BUILD
        memset((uint8_t *)list->data + remaining_bytes, AWS_ARRAY_LIST_DEBUG_FILL, popping_bytes);
#endif
    }
    AWS_POSTCONDITION(aws_array_list_is_valid(list));
}