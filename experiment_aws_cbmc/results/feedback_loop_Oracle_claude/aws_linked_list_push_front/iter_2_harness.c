#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness(void) {
    /* Allocate and initialize the linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Precondition: list must be valid after init */
    assert(aws_linked_list_is_valid(&list));

    /* Allocate a node to push to the front */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    assert(node != NULL);

    /* Use a nondet boolean to decide if we pre-populate the list */
    bool list_has_element;
    struct aws_linked_list_node *existing_node = NULL;

    if (list_has_element) {
        existing_node = malloc(sizeof(struct aws_linked_list_node));
        assert(existing_node != NULL);
        aws_linked_list_push_back(&list, existing_node);
        assert(aws_linked_list_is_valid(&list));
        assert(!aws_linked_list_empty(&list));
    }

    /* Save state before the call */
    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* Call the function under verification */
    aws_linked_list_push_front(&list, node);

    /* Postcondition 1: The list is still valid after push_front */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 2: The list is not empty after push_front */
    assert(!aws_linked_list_empty(&list));

    /* Postcondition 3: node is now the first element of the list */
    assert(list.head.next == node);

    /* Postcondition 4: node's prev points to head */
    assert(node->prev == &list.head);

    /* Postcondition 5: node's next points to what was previously the first element */
    assert(node->next == old_head_next);

    /* Postcondition 6: The old first element's prev now points to node */
    assert(old_head_next->prev == node);

    /* Postcondition 7: head's next points to node */
    assert(list.head.next == node);

    /* Postcondition 8: head is still valid */
    assert(list.head.prev == NULL);
    assert(list.head.next != NULL);

    /* Postcondition 9: tail is still valid */
    assert(list.tail.next == NULL);
    assert(list.tail.prev != NULL);

    /* Postcondition 10: If the list was empty before, node's next is tail */
    if (was_empty) {
        assert(node->next == &list.tail);
        assert(list.tail.prev == node);
    }

    /* Postcondition 11: If the list was non-empty before, tail is unchanged */
    if (!was_empty && existing_node != NULL) {
        assert(list.tail.prev == existing_node);
        assert(existing_node->next == &list.tail);
    }

    /* Postcondition 12: front() returns node */
    assert(aws_linked_list_front(&list) == node);
}
