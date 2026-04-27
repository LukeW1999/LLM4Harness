aws_array_list_clean_up(struct aws_array_list *AWS_RESTRICT list) {
    if (list->alloc && list->data) {
        aws_mem_release(list->alloc, list->data);
    }

    AWS_ZERO_STRUCT(*list);
}