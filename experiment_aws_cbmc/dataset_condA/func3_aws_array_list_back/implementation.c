/* From: include/aws/common/array_list.inl */

AWS_STATIC_IMPL
int aws_array_list_back(const struct aws_array_list *AWS_RESTRICT list, void *val) {
    AWS_PRECONDITION(aws_array_list_is_valid(list));
    AWS_PRECONDITION(
        val && AWS_MEM_IS_WRITABLE(val, list->item_size),
        "Input pointer [val] must point writable memory of [list->item_size] bytes.");
    if (aws_array_list_length(list) > 0) {
        size_t last_item_offset = list->item_size * (aws_array_list_length(list) - 1);

        memcpy(val, (void *)((uint8_t *)list->data + last_item_offset), list->item_size);
        AWS_POSTCONDITION(aws_array_list_is_valid(list));
        return AWS_OP_SUCCESS;
    }

    AWS_POSTCONDITION(aws_array_list_is_valid(list));
    return aws_raise_error(AWS_ERROR_LIST_EMPTY);
}
