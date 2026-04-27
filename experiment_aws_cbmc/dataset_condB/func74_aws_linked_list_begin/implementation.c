aws_linked_list_begin(const struct aws_linked_list *list) {
    struct aws_linked_list_node *rval = list->head.next;
    return rval;
}