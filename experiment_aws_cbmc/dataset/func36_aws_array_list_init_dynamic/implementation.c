aws_array_list_init_dynamic(
    struct aws_array_list *AWS_RESTRICT list,
    struct aws_allocator *alloc,
    size_t initial_item_allocation,
    size_t item_size) {

    AWS_FATAL_PRECONDITION(list != NULL);
    AWS_FATAL_PRECONDITION(alloc != NULL);
    AWS_FATAL_PRECONDITION(item_size > 0);

    AWS_ZERO_STRUCT(*list);

    size_t allocation_size = 0;
    if (aws_mul_size_checked(initial_item_allocation, item_size, &allocation_size)) {
        goto error;
    }

    if (allocation_size > 0) {
        list->data = aws_mem_acquire(alloc, allocation_size);
        if (!list->data) {
            goto error;
        }
#ifdef DEBUG_BUILD
        memset(list->data, AWS_ARRAY_LIST_DEBUG_FILL, allocation_size);

#endif
        list->current_size = allocation_size;
    }
    list->item_size = item_size;
    list->alloc = alloc;

    AWS_FATAL_POSTCONDITION(list->current_size == 0 || list->data);
    AWS_POSTCONDITION(aws_array_list_is_valid(list));
    return AWS_OP_SUCCESS;

error:
    AWS_POSTCONDITION(AWS_IS_ZEROED(*list));
    return AWS_OP_ERR;
}