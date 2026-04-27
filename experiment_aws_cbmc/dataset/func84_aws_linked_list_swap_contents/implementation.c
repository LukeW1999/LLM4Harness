aws_linked_list_swap_contents(
    struct aws_linked_list *AWS_RESTRICT a,
    struct aws_linked_list *AWS_RESTRICT b) {

    AWS_PRECONDITION(aws_linked_list_is_valid(a));
    AWS_PRECONDITION(aws_linked_list_is_valid(b));
    AWS_PRECONDITION(a != b);
    struct aws_linked_list_node *a_first = a->head.next;
    struct aws_linked_list_node *a_last = a->tail.prev;

    /* Move B's contents into A */
    if (aws_linked_list_empty(b)) {
        aws_linked_list_init(a);
    } else {
        a->head.next = b->head.next;
        a->head.next->prev = &a->head;
        a->tail.prev = b->tail.prev;
        a->tail.prev->next = &a->tail;
    }

    /* Move A's old contents into B */
    if (a_first == &a->tail) {
        aws_linked_list_init(b);
    } else {
        b->head.next = a_first;
        b->head.next->prev = &b->head;
        b->tail.prev = a_last;
        b->tail.prev->next = &b->tail;
    }
    AWS_POSTCONDITION(aws_linked_list_is_valid(a));
    AWS_POSTCONDITION(aws_linked_list_is_valid(b));
}