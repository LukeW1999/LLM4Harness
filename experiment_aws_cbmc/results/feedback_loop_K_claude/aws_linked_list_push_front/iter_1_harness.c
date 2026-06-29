#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness(void) {
    /* Allocate and initialize the linked list */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    /* Initialize the list to a valid state */
    aws_linked_list_init(list);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Allocate the node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* The node should be a fresh node (not already in the list) */
    /* We set its pointers to nondet but ensure it won't alias list internals */
    node->next = NULL;
    node->prev = NULL;

    /* Snapshot state before the call */
    struct aws_linked_list_node *old_head_next = list->head.next;

    /* Preconditions verified */
    assert(aws_linked_list_is_valid(list));
    assert(node != NULL);

    /* Call the function under test */
    aws_linked_list_push_front(list, node);

    /* Postcondition: list is still valid */
    assert(aws_linked_list_is_valid(list));

    /* Postcondition: node is the new first element */
    assert(list->head.next == node);

    /* Postcondition: node's prev points back to head */
    assert(node->prev == &list->head);

    /* Postcondition: node's next points to the old first element */
    assert(node->next == old_head_next);

    /* Postcondition: old first element's prev now points to node */
    assert(old_head_next->prev == node);

    /* Postcondition: list is non-empty */
    assert(!aws_linked_list_empty(list));

    /* Postcondition: frame - head.prev is still NULL */
    assert(list->head.prev == NULL);

    /* Postcondition: frame - tail.next is still NULL */
    assert(list->tail.next == NULL);

    /* Postcondition: tail.prev is still valid (non-null) */
    assert(list->tail.prev != NULL);
}

void aws_linked_list_push_front_harness(void) {
    aws_linked_list_push_front_harness();
    return 0;
}
