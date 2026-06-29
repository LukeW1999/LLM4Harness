#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_pop_back_harness(void) {
    /* Allocate the list */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    /* Initialize the list to a valid empty state */
    aws_linked_list_init(list);

    /* We need at least one node in the list.
     * Allocate one or two nodes non-deterministically to keep the
     * state space manageable while covering both single-element and
     * multi-element cases. */
    struct aws_linked_list_node *node1 = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node1 != NULL);

    /* Push the first node onto the back */
    aws_linked_list_push_back(list, node1);

    /* Non-deterministically add a second node */
    bool add_second;
    if (add_second) {
        struct aws_linked_list_node *node2 = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node2 != NULL);
        aws_linked_list_push_back(list, node2);
    }

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(list));
    __CPROVER_assume(!aws_linked_list_empty(list));

    /* Remember the expected back node before the call */
    struct aws_linked_list_node *expected_back = list->tail.prev;
    __CPROVER_assume(expected_back != NULL);
    __CPROVER_assume(expected_back != &list->head);
    __CPROVER_assume(expected_back != &list->tail);

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(list);

    /* Postconditions */

    /* Return value must not be NULL */
    assert(result != NULL);

    /* Return value must be the node that was at the back */
    assert(result == expected_back);

    /* The returned node must have been reset (next and prev are NULL) */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* The list must still be valid after the operation */
    assert(aws_linked_list_is_valid(list));

    /* list->head.prev must remain NULL (invariant of the list structure) */
    assert(list->head.prev == NULL);

    /* list->tail.next must remain NULL (invariant of the list structure) */
    assert(list->tail.next == NULL);

    /* The returned node must no longer be in the list:
     * its next and prev are NULL, confirming removal */
    assert(!aws_linked_list_node_is_in_list(result));

    /* The new tail.prev must not be the removed node */
    assert(list->tail.prev != result);

    /* The new head.next must not be the removed node */
    assert(list->head.next != result);
}

void aws_linked_list_pop_back_harness(void) {
    aws_linked_list_pop_back_harness();
    return 0;
}
