#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_prev_harness(void) {
    /* Allocate a linked list and two nodes to form a minimal valid structure */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Create a node to insert into the list */
    struct aws_linked_list_node node;

    /* Insert node into the list so it has valid prev/next pointers */
    aws_linked_list_push_back(&list, &node);

    /* Verify the list is valid before calling the function */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_prev_is_valid(&node));

    /* Save state before the call for frame condition checks */
    struct aws_linked_list_node *node_prev_before = node.prev;
    struct aws_linked_list_node *node_next_before = node.next;
    struct aws_linked_list_node *head_next_before = list.head.next;
    struct aws_linked_list_node *tail_prev_before = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* Postcondition 1: Return value correctness
     * The result must equal node->prev (which was set before the call) */
    assert(result == node_prev_before);

    /* Postcondition 2: The result must not be NULL
     * Since node is in the list, node->prev points to list.head at minimum */
    assert(result != NULL);

    /* Postcondition 3: The result should be the head of the list
     * since node was the only element pushed to back */
    assert(result == &list.head);

    /* Postcondition 4: Frame conditions - verify nothing was modified */
    assert(node.prev == node_prev_before);
    assert(node.next == node_next_before);
    assert(list.head.next == head_next_before);
    assert(list.tail.prev == tail_prev_before);

    /* Postcondition 5: The list remains valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 6: The result's next pointer should point back to node
     * (bidirectional linkage invariant) */
    assert(result->next == &node);

    /* Additional test: verify with the tail node */
    struct aws_linked_list_node *tail_prev_before2 = list.tail.prev;
    struct aws_linked_list_node *result2 = aws_linked_list_prev(&list.tail);

    /* The prev of tail should be the last node in the list (our node) */
    assert(result2 == tail_prev_before2);
    assert(result2 != NULL);
    assert(result2 == &node);

    /* Frame condition: tail's prev unchanged */
    assert(list.tail.prev == tail_prev_before2);

    /* List still valid */
    assert(aws_linked_list_is_valid(&list));
}
