#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_front_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate at least one node to ensure the list is non-empty */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Push the node to the front so the list is non-empty */
    aws_linked_list_push_front(&list, node);

    /* Precondition: list must not be empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save state before the call for frame condition checks */
    struct aws_linked_list_node *head_next_before = list.head.next;
    struct aws_linked_list_node *head_prev_before = list.head.prev;
    struct aws_linked_list_node *tail_next_before = list.tail.next;
    struct aws_linked_list_node *tail_prev_before = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* Postcondition 1: Return value correctness
       The returned node must be the first node (head.next) */
    assert(result == list.head.next);

    /* Postcondition 2: The returned node must not be NULL
       (since the list is non-empty, head.next != &list.tail) */
    assert(result != NULL);

    /* Postcondition 3: The returned node must not be the tail sentinel */
    assert(result != &list.tail);

    /* Postcondition 4: The returned node's prev must point back to head */
    assert(result->prev == &list.head);

    /* Postcondition 5: Frame condition - list structure is not modified */
    assert(list.head.next == head_next_before);
    assert(list.head.prev == head_prev_before);
    assert(list.tail.next == tail_next_before);
    assert(list.tail.prev == tail_prev_before);

    /* Postcondition 6: List validity is preserved */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 7: List is still non-empty after the call */
    assert(!aws_linked_list_empty(&list));
}
