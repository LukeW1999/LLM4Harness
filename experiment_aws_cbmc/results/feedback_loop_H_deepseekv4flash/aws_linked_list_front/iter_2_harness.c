#include <aws/common/linked_list.h>
#include <stdlib.h>

void aws_linked_list_front_harness() {
    /* Allocate the list structure */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    /* Allocate head and tail sentinels (they are part of the list struct) */
    /* In AWS C common, head and tail are embedded in the list struct */
    /* So we don't need to allocate them separately */

    /* Allocate at least one node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Set up the list with one node */
    /* Head sentinel: head.next points to node, head.prev points to tail */
    list->head.next = node;
    list->head.prev = &list->tail;

    /* Node: next points to tail, prev points to head */
    node->next = &list->tail;
    node->prev = &list->head;

    /* Tail sentinel: tail.next points to head, tail.prev points to node */
    list->tail.next = &list->head;
    list->tail.prev = node;

    /* Ensure the list is valid and non-empty */
    __CPROVER_assume(aws_linked_list_is_valid_deep(list));
    __CPROVER_assume(!aws_linked_list_empty(list));

    /* Save old state */
    struct aws_linked_list old_list = *list;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(list);

    /* Postcondition: result points to the first node */
    assert(result == list->head.next);

    /* Postcondition: list structure is unchanged */
    assert(list->head.next == old_list.head.next);
    assert(list->head.prev == old_list.head.prev);
    assert(list->tail.next == old_list.tail.next);
    assert(list->tail.prev == old_list.tail.prev);

    /* Postcondition: result is a valid node (not NULL, not the tail sentinel) */
    assert(result != NULL);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));

    /* Invariant: list remains valid */
    assert(aws_linked_list_is_valid_deep(list));
}
