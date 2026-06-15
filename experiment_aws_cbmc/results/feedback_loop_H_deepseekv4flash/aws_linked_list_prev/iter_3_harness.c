#include <aws/common/linked_list.h>
#include <stdlib.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    if (!list) return;
    list->head.prev = &list->tail;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = &list->head;

    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    if (!node) { free(list); return; }

    // Insert one node between head and tail
    list->head.next = node;
    node->prev = &list->head;
    node->next = &list->tail;
    list->tail.prev = node;

    __CPROVER_assume(node != &list->head);
    __CPROVER_assume(list->head.next == node);
    __CPROVER_assume(list->head.prev == &list->tail);
    __CPROVER_assume(list->tail.next == &list->head);
    __CPROVER_assume(list->tail.prev == node);

    struct aws_linked_list_node *prev = aws_linked_list_prev(node);

    assert(prev == node->prev);
    assert(prev != NULL);
    assert(list->head.next == node);
    assert(list->head.prev == &list->tail);
    assert(list->tail.next == &list->head);
    assert(list->tail.prev == node);
    assert(node->prev == &list->head);
    assert(node->next == &list->tail);

    free(node);
    free(list);
}
