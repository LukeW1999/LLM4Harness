aws_array_list_erase(struct aws_array_list *AWS_RESTRICT list, size_t index) {

    const size_t length = aws_array_list_length(list);

    if (index >= length) {
        return aws_raise_error(AWS_ERROR_INVALID_INDEX);
    }

    if (index == 0) {
        /* Removing front element */
        aws_array_list_pop_front(list);
    } else if (index == (length - 1)) {
        /* Removing back element */
        aws_array_list_pop_back(list);
    } else {
        /* Removing middle element */
        uint8_t *item_ptr = (uint8_t *)list->data + (index * list->item_size);
        uint8_t *next_item_ptr = item_ptr + list->item_size;
        size_t trailing_items = (length - index) - 1;
        size_t trailing_bytes = trailing_items * list->item_size;
        memmove(item_ptr, next_item_ptr, trailing_bytes);

        aws_array_list_pop_back(list);
    }

    return AWS_OP_SUCCESS;
}