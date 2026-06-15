#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_linked_list_prev_harness() {
    /* Allocate and initialize a linked list with a sentinel head and one real node */
    struct aws_linked_list *list = (struct aws_linked_list *)malloc(sizeof(struct aws_linked_list));
    if (!list) return;
    list->head.prev = &list->tail;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = &list->head;

    /* Allocate one real node */
    struct aws_linked_list_node *node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    if (!node) { free(list); return; }
    /* Insert node between head and tail */
    /* head -> node -> tail */
    list->head.next = node;
    node->prev = &list->head;
    node->next = &list->tail;
    list->tail.prev = node;

    /* Ensure the list is valid and non-empty */
    __CPROVER_assume(aws_linked_list_is_valid(list));
    __CPROVER_assume(!aws_linked_list_empty(list));

    /* Save old list state (immutability check) */
    struct aws_linked_list old_list = *list;

    /* Call the function under test on the real node */
    struct aws_linked_list_node *prev = aws_linked_list_prev(node);

    /* Postcondition: prev equals node->prev */
    assert(prev == node->prev);

    /* No modifications to the list structure */
    assert(list->head.next == old_list.head.next);
    assert(list->head.prev == old_list.head.prev);
    assert(list->tail.next == old_list.tail.next);
    assert(list->tail.prev == old_list.tail.prev);

    /* List validity preserved */
    assert(aws_linked_list_is_valid(list));

    /* Clean up */
    free(node);
    free(list);
}
