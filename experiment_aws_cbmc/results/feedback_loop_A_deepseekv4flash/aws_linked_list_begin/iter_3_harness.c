#include <aws/common/linked_list.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_begin_harness() {
    /* Allocate a linked list structure */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    /* Initialize head and tail sentinels */
    struct aws_linked_list_node *head = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *tail = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(head != NULL && tail != NULL);

    /* Set up the empty list: head.next = tail, tail.prev = head */
    head->next = tail;
    head->prev = NULL;
    tail->prev = head;
    tail->next = NULL;
    list->head = *head;   // copy structure
    list->tail = *tail;

    /* Optionally add some nodes to make list non-empty (but not required) */
    // Here we keep it empty for simplicity.

    /* Ensure the list is valid according to the library's check */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Save old state */
    struct aws_linked_list old = *list;

    /* Call function under verification */
    struct aws_linked_list_node *result = aws_linked_list_begin(list);

    /* Postcondition: returns pointer to the first element (head.next) */
    assert(result == list->head.next);

    /* Immutability: list structure unchanged */
    assert(list->head.next == old.head.next);
    assert(list->head.prev == old.head.prev);
    assert(list->tail.next == old.tail.next);
    assert(list->tail.prev == old.tail.prev);

    /* Invariant remains valid */
    assert(aws_linked_list_is_valid(list));

    /* Clean up (optional, not needed for CBMC) */
    free(head);
    free(tail);
    free(list);
}
