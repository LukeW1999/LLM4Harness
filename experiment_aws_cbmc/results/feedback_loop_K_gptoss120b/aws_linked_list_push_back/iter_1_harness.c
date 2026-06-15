/*************************************************************
 * Contract for aws_linked_list_push_back
 *
 * Preconditions:
 *   - list is a valid aws_linked_list (circular sentinel structure intact):
 *       list->head.prev == NULL
 *       list->tail.next == NULL
 *       list->head.next == &list->tail
 *       list->tail.prev points to the current last element (or &list->head if empty)
 *   - node != NULL
 *
 * Postconditions (validity):
 *   - list remains a valid aws_linked_list
 *   - list->tail.prev == node   // node is the new last element
 *   - node->next == &list->tail
 *   - node->prev == old_tail_prev (the previous last element)
 *   - old_tail_prev->next == node   // if the list was not empty
 *   - node->next->prev == node
 *
 * Frame conditions:
 *   - No memory outside of {list, node, old_tail_prev} is modified.
 *   - The contents of list->head and list->tail (except tail.prev) remain unchanged.
 *************************************************************/

#include <aws/common/linked_list.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    /* Initialize sentinel nodes */
    list.head.prev = NULL;
    list.head.next = &list.tail;
    list.tail.prev = &list.head;
    list.tail.next = NULL;

    /* Assume the list is valid */
    __CPROVER_assume(list.head.prev == NULL);
    __CPROVER_assume(list.tail.next == NULL);
    __CPROVER_assume(list.head.next == &list.tail);
    __CPROVER_assume(list.tail.prev == &list.head);

    /* Allocate a node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Preserve old state for frame checks */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* Postcondition: list remains valid */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == node);

    /* Postcondition: node links are correct */
    assert(node->next == &list.tail);
    assert(node->prev == old_tail_prev);
    assert(node->next->prev == node);
    assert(node->prev->next == node);

    /* Frame conditions: unchanged parts of the list */
    assert(list.head.prev == old_head_prev);
    assert(list.head.next == old_head_next);
    assert(list.tail.next == old_tail_next);
    /* The only changed pointer in the tail sentinel is prev, already checked */

    /* No other memory modifications (node's next/prev are the only fields set) */
    return 0;
}
