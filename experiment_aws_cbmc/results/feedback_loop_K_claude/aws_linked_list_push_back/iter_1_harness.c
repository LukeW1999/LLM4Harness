#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Manually set up a valid aws_linked_list (head <-> tail sentinel structure).
 * head.prev == NULL, tail.next == NULL,
 * head.next == &tail, tail.prev == &head  (empty list).
 */
static void setup_valid_empty_list(struct aws_linked_list *list) {
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

/*
 * Check aws_linked_list_is_valid predicate manually:
 *   - head.prev == NULL
 *   - tail.next == NULL
 *   - head.next->prev == &head  (circular linkage from head side)
 *   - tail.prev->next == &tail  (circular linkage from tail side)
 */
static bool linked_list_is_valid(struct aws_linked_list *list) {
    return (list != NULL) &&
           (list->head.prev == NULL) &&
           (list->tail.next == NULL) &&
           (list->head.next != NULL) &&
           (list->head.next->prev == &list->head) &&
           (list->tail.prev != NULL) &&
           (list->tail.prev->next == &list->tail);
}

void aws_linked_list_push_back_harness(void) {
    /* Allocate and initialize the list */
    struct aws_linked_list *list =
        (struct aws_linked_list *)malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    /* Set up a valid (empty) list */
    setup_valid_empty_list(list);

    /* Precondition: list must be valid before the call */
    __CPROVER_assume(linked_list_is_valid(list));

    /* Allocate a new node (contents are non-deterministic, which is fine) */
    struct aws_linked_list_node *node =
        (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* node must not alias list internals */
    __CPROVER_assume(node != &list->head);
    __CPROVER_assume(node != &list->tail);

    /* Save the old tail predecessor for frame condition checks */
    struct aws_linked_list_node *old_tail_prev = list->tail.prev;

    /* Call the function under verification */
    aws_linked_list_push_back(list, node);

    /* Postcondition 1: list is still valid */
    assert(linked_list_is_valid(list));

    /* Postcondition 2: node is the new last element (just before tail) */
    assert(list->tail.prev == node);

    /* Postcondition 3: node->next points to tail sentinel */
    assert(node->next == &list->tail);

    /* Postcondition 4: node->prev->next == node (forward linkage of node) */
    assert(node->prev != NULL);
    assert(node->prev->next == node);

    /* Postcondition 5: node->prev is the old tail predecessor */
    assert(node->prev == old_tail_prev);

    /* Postcondition 6: old tail predecessor now points forward to node */
    assert(old_tail_prev->next == node);

    /* Frame condition: head.prev is still NULL */
    assert(list->head.prev == NULL);

    /* Frame condition: tail.next is still NULL */
    assert(list->tail.next == NULL);

    /* Frame condition: head.next->prev still points back to head */
    assert(list->head.next != NULL);
    assert(list->head.next->prev == &list->head);
}
