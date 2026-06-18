#include <proof_helpers/make_common_data_structures.h>

static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        len++;
        cur = cur->next;
    }
    return len;
}

void harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    struct aws_linked_list_node node;
    node.prev = NULL;
    node.next = NULL;

    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    size_t len_before = list_length(&list);

    aws_linked_list_push_back(&list, &node);

    __CPROVER_assert(aws_linked_list_is_valid(&list), "list must be valid");
    __CPROVER_assert(aws_linked_list_node_is_valid(&node), "new node must be valid");
    __CPROVER_assert(list.tail.prev == &node, "tail.prev must point to new node");
    __CPROVER_assert(node.next == &list.tail, "new node next points to tail");
    __CPROVER_assert(node.prev == old_tail_prev, "new node prev points to previous tail predecessor");
    __CPROVER_assert(old_tail_prev->next == &node, "previous tail predecessor links to new node");
    __CPROVER_assert(list.head.prev == NULL, "head.prev unchanged");
    __CPROVER_assert(list.tail.next == NULL, "tail.next unchanged");
    __CPROVER_assert(list_length(&list) == len_before + 1, "list length increased by one");
}
