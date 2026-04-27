aws_linked_list_front(const struct aws_linked_list *list) {
    struct aws_linked_list_node *rval = list->head.next;
    return rval;
}