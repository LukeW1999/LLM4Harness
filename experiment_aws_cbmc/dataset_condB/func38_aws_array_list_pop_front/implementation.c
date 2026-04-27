aws_array_list_pop_front(struct aws_array_list *AWS_RESTRICT list) {
    if (aws_array_list_length(list) > 0) {
        aws_array_list_pop_front_n(list, 1);
        return AWS_OP_SUCCESS;
    }

    return aws_raise_error(AWS_ERROR_LIST_EMPTY);
}