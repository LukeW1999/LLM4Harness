#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_pop_front_harness(void) {
    /* Allocate and initialize the linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate one or two nodes to add to the list so it's non-empty */
    struct aws_linked_list_node *node1 = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node1 != NULL);

    /* Optionally add a second node (non-deterministic) */
    bool add_second;
    struct aws_linked_list_node *node2 = NULL;

    /* Push node1 to the back */
    aws_linked_list_push_back(&list, node1);

    if (add_second) {
        node2 = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node2 != NULL);
        aws_linked_list_push_back(&list, node2);
    }

    /* Preconditions: list must be valid and non-empty */
    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));

    /* Save state before the call */
    struct aws_linked_list_node *expected_front = list.head.next;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    /* Postcondition 1: Return value correctness
     * The returned node should be the node that was at the front */
    assert(result == expected_front);
    assert(result != NULL);

    /* Postcondition 2: The returned node's next and prev are NULL
     * (it has been reset/removed from the list) */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* Postcondition 3: The list is still valid after the operation */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 4: The returned node is no longer in the list
     * (it should not be reachable from head) */
    assert(!aws_linked_list_node_is_in_list(result));

    /* Postcondition 5: Frame condition - list head and tail sentinel
     * nodes maintain their invariants */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);

    /* Postcondition 6: If we added two nodes, the second node should
     * now be the front of the list */
    if (add_second && node2 != NULL) {
        assert(list.head.next == node2);
        assert(node2->prev == &list.head);
        assert(!aws_linked_list_empty(&list));
    } else {
        /* If only one node was added, list should now be empty */
        assert(aws_linked_list_empty(&list));
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    }

    /* Postcondition 7: The returned node was indeed node1 (the first pushed) */
    assert(result == node1);
}

void aws_linked_list_pop_front_harness(void) {
    aws_linked_list_pop_front_harness();
    return 0;
}
