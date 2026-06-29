#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;

    /* Allocate and set up nodes non-deterministically */
    /* We need to set up a valid linked list */
    /* Use nondet to decide if list is empty or has elements */
    
    /* Initialize the list properly */
    aws_linked_list_init(&list);
    
    /* Optionally add some nodes */
    struct aws_linked_list_node *node1 = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *node2 = malloc(sizeof(struct aws_linked_list_node));
    
    bool add_node1;
    bool add_node2;
    
    if (node1 != NULL && add_node1) {
        aws_linked_list_push_back(&list, node1);
    }
    if (node2 != NULL && add_node2) {
        aws_linked_list_push_back(&list, node2);
    }

    /* Precondition */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save state before call */
    struct aws_linked_list_node *tail_prev_before = list.tail.prev;
    struct aws_linked_list_node *head_next_before = list.head.next;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    /* Postcondition 1: Return value correctness
       rbegin returns list->tail.prev, which is the last element
       (or head if list is empty) */
    assert(result == tail_prev_before);

    /* Postcondition 2: The result is not NULL (tail.prev is always valid in a valid list) */
    assert(result != NULL);

    /* Postcondition 3: The list structure is unchanged (frame condition) */
    assert(list.tail.prev == tail_prev_before);
    assert(list.head.next == head_next_before);

    /* Postcondition 4: The list is still valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 5: If the list is empty, rbegin returns &list.head */
    if (aws_linked_list_empty(&list)) {
        assert(result == &list.head);
    }

    /* Postcondition 6: If the list is not empty, result->next == &list.tail */
    if (!aws_linked_list_empty(&list)) {
        assert(result->next == &list.tail);
    }
}

void aws_linked_list_rbegin_harness(void) {
    aws_linked_list_rbegin_harness();
    return 0;
}
