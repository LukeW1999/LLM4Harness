#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_begin_harness(void) {
    /* Allocate a linked list */
    struct aws_linked_list list;

    /* Set up the list with non-deterministic but valid structure */
    /* We need to create a valid linked list */
    /* For simplicity, use an empty list or a list with one node */
    
    /* Initialize the list to make it valid */
    aws_linked_list_init(&list);

    /* Optionally add a node non-deterministically */
    bool add_node;
    struct aws_linked_list_node node;
    
    if (add_node) {
        aws_linked_list_push_back(&list, &node);
    }

    /* Precondition */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save state before call for frame condition checks */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* Postcondition 1: Return value correctness
       The result should be list.head.next */
    assert(result == old_head_next);

    /* Postcondition 2: The result is not NULL (head.next is always valid in a valid list) */
    assert(result != NULL);

    /* Postcondition 3: The result points to either a real node or the tail
       (i.e., it's a valid pointer within the list structure) */
    /* If the list is empty, result should be &list.tail */
    if (aws_linked_list_empty(&list)) {
        assert(result == &list.tail);
    } else {
        /* If non-empty, result should not be &list.tail */
        assert(result != &list.tail);
    }

    /* Postcondition 4: Frame conditions - the list structure is not modified */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* Postcondition 5: The list is still valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 6: The result's prev pointer points back to head
       (since begin returns head.next, and in a valid list head.next->prev == &head) */
    assert(result->prev == &list.head);
}
