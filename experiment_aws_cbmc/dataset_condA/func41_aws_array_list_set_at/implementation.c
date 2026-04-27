aws_array_list_set_at(struct aws_array_list *AWS_RESTRICT list, const void *val, size_t index) {
    AWS_PRECONDITION(
        val && AWS_MEM_IS_READABLE(val, list->item_size),
        "Input pointer [val] must point readable memory of [list->item_size] bytes.");

    if (aws_array_list_ensure_capacity(list, index)) {
        return AWS_OP_ERR;
    }

    AWS_FATAL_PRECONDITION(list->data);

    memcpy((void *)((uint8_t *)list->data + (list->item_size * index)), val, list->item_size);

    /*
     * This isn't perfect, but its the best I can come up with for detecting
     * length changes.
     */
    if (index >= aws_array_list_length(list)) {
        if (aws_add_size_checked(index, 1, &list->length)) {
            return AWS_OP_ERR;
        }
    }

    return AWS_OP_SUCCESS;
}