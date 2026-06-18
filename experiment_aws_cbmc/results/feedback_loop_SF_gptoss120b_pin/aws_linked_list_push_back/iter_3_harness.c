#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_back_harness(void) {
    struct aws_linked_list list;

    /* manually initialize the list sentinel nodes */
    list.head.prev = &list.head;
    list.head.next = &list.tail;
    list.tail.prev = &list.head;
    list.tail.next = &list.tail;

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    node->prev = NULL;
    node->next = NULL;

    struct aws_linked_list_node *orig_tail_prev = list.tail.prev;

    aws_linked_list_push_back(&list, node);

    /* post‑conditions */
    assert(list.tail.prev == node);
    assert(node->prev == orig_tail_prev);
    assert(node->next == &list.tail);
    assert(orig_tail_prev->next == node);
    assert(node->prev->next == node);
    assert(node->next->prev == node);
    assert(list.tail.next == &list.tail);
    assert(list.head.prev == &list.head);
}
