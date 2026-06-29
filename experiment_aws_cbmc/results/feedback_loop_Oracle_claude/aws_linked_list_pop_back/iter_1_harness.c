#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_pop_back_harness(void) {
    /* Allocate a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate one or two nodes to add to the list (non-deterministic count) */
    struct aws_linked_list_node *node1 = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node1 != NULL);

    struct aws_linked_list_node *node2 = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node2 != NULL);

    /* Non-deterministically choose to add one or two nodes */
    bool add_second_node;

    /* Always add at least one node (precondition: list must not be empty) */
    aws_linked_list_push_back(&list, node1);

    if (add_second_node) {
        aws_linked_list_push_back(&list, node2);
    }

    /* Snapshot state before the call */
    bool was_single_element = (list.tail.prev == node1 && list.head.next == node1);
    struct aws_linked_list_node *expected_back = list.tail.prev;
    struct aws_linked_list_node *expected_new_back = expected_back->prev;

    /* Preconditions */
    assert(!aws_linked_list_empty(&list));
    assert(aws_linked_list_is_valid(&list));

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    /* Postcondition 1: Return value correctness
     * The returned node should be the one that was at the back */
    assert(result == expected_back);
    assert(result != NULL);

    /* Postcondition 2: The returned node's next and prev are NULL
     * (it has been reset/removed from the list) */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* Postcondition 3: The list is still valid after the operation */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 4: The new back of the list is what was previously
     * the second-to-last element */
    assert(list.tail.prev == expected_new_back);

    /* Postcondition 5: The new back's next pointer points to tail */
    assert(list.tail.prev->next == &list.tail);

    /* Postcondition 6: The tail's prev pointer is correctly updated */
    assert(list.tail.prev == expected_new_back);

    /* Postcondition 7: If the list had only one element, it should now be empty */
    if (was_single_element) {
        assert(aws_linked_list_empty(&list));
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    }

    /* Postcondition 8: If the list had two elements, it should now have one */
    if (add_second_node && !was_single_element) {
        assert(!aws_linked_list_empty(&list));
    }

    /* Postcondition 9: Head's prev is still NULL (invariant) */
    assert(list.head.prev == NULL);

    /* Postcondition 10: Tail's next is still NULL (invariant) */
    assert(list.tail.next == NULL);

    /* Postcondition 11: The returned node is not in the list anymore */
    assert(!aws_linked_list_node_is_in_list(result));

    /* Postcondition 12: The list head's next is valid */
    assert(list.head.next != NULL);

    /* Postcondition 13: The list tail's prev is valid */
    assert(list.tail.prev != NULL);

    /* Postcondition 14: Bidirectional connectivity at the new tail */
    assert(list.tail.prev->next == &list.tail);
    assert(list.tail.next == NULL);
}

void aws_linked_list_pop_back_harness(void) {
    aws_linked_list_pop_back_harness();
    return 0;
}
