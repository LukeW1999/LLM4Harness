#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_back_harness(void) {
    /* Allocate and initialize the linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate at least one node to ensure the list is non-empty */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_push_back(&list, node);

    /* Optionally add a second node non-deterministically */
    struct aws_linked_list_node *node2 = malloc(sizeof(struct aws_linked_list_node));
    if (node2 != NULL) {
        bool add_second;
        if (add_second) {
            aws_linked_list_push_back(&list, node2);
        }
    }

    /* Precondition: list must not be empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save state before call for frame condition checks */
    struct aws_linked_list_node *tail_prev_before = list.tail.prev;
    struct aws_linked_list_node *head_next_before = list.head.next;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* Postcondition 1: Return value is not NULL (list is non-empty) */
    assert(result != NULL);

    /* Postcondition 2: Return value equals tail.prev */
    assert(result == list.tail.prev);

    /* Postcondition 3: Return value is not the head sentinel */
    assert(result != &list.head);

    /* Postcondition 4: Return value is not the tail sentinel */
    assert(result != &list.tail);

    /* Postcondition 5: The returned node's next pointer points to tail */
    assert(result->next == &list.tail);

    /* Postcondition 6: Frame condition - list structure not modified */
    assert(list.tail.prev == tail_prev_before);
    assert(list.head.next == head_next_before);

    /* Postcondition 7: List is still valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 8: List is still non-empty after the call */
    assert(!aws_linked_list_empty(&list));
}
