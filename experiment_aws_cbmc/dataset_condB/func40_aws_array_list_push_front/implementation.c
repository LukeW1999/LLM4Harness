aws_array_list_push_front(struct aws_array_list *AWS_RESTRICT list, const void *val) {
    AWS_PRECONDITION(
        val && AWS_MEM_IS_READABLE(val, list->item_size),
        "Input pointer [val] must point writable memory of [list->item_size] bytes.");
    size_t orig_len = aws_array_list_length(list);
    int err_code = aws_array_list_ensure_capacity(list, orig_len);

    if (err_code && aws_last_error() == AWS_ERROR_INVALID_INDEX && !list->alloc) {
        return aws_raise_error(AWS_ERROR_LIST_EXCEEDS_MAX_SIZE);
    } else if (err_code) {
        return err_code;
    }
    if (orig_len) {
        memmove((uint8_t *)list->data + list->item_size, list->data, orig_len * list->item_size);
    }
    ++list->length;
    memcpy(list->data, val, list->item_size);

    return err_code;
}