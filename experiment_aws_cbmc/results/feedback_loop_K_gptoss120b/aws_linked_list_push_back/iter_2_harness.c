/*************************************************************
 * Harness for aws_linked_list_push_back
 *
 * This harness verifies the behavior of aws_linked_list_push_back
 * on an initially empty list.
 *************************************************************/

#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_back_harness(void) {
    /* Initialize an empty linked list (sentinel nodes only) */
    struct aws_linked_list list;
    list.head.prev = NULL;
    list.head.next = &list.tail;
    list.tail.prev = &list.head;
    list.tail.next = NULL;

    /* Assume the list is in a valid empty state */
    __CPROVER_assume(list.head.prev == NULL);
    __CPROVER_assume(list.tail.next == NULL);
    __CPROVER_assume(list.head.next == &list.tail);
    __CPROVER_assume(list.tail.prev == &list.head);

    /* Allocate a node to be pushed */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Preserve the original sentinel pointers for frame checks */
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* Postconditions: list remains a valid empty‑to‑non‑empty transition */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next == node);          /* first element is the new node */
    assert(list.tail.prev == node);          /* last element is the new node */

    /* Node links are correctly set */
    assert(node->next == &list.tail);
    assert(node->prev == &list.head);
    assert(node->next->prev == node);
    assert(node->prev->next == node);

    /* Frame conditions: unchanged parts of the sentinel nodes */
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
}
