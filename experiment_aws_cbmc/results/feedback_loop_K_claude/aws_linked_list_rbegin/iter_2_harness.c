#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness(void) {
    /* Stack-allocate a linked list */
    struct aws_linked_list list;

    /* Initialize the list so it satisfies aws_linked_list_is_valid */
    aws_linked_list_init(&list);

    /* Optionally add one node to allow both empty and non-empty cases */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    bool add_node;
    if (add_node) {
        aws_linked_list_push_back(&list, node);
    }

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save state before call for frame condition checks */
    struct aws_linked_list_node *tail_prev_before = list.tail.prev;
    struct aws_linked_list_node *head_next_before = list.head.next;
    struct aws_linked_list_node *tail_next_before = list.tail.next;
    struct aws_linked_list_node *head_prev_before = list.head.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    /* Postcondition: result is non-null */
    assert(result != NULL);

    /* Postcondition: result equals list.tail.prev */
    assert(result == list.tail.prev);

    /* Postcondition: if list is empty, result should be &list.head */
    if (aws_linked_list_empty(&list)) {
        assert(result == &list.head);
    } else {
        /* Postcondition: if non-empty, result is not the head sentinel */
        assert(result != &list.head);
        /* result should be a node whose next is &list.tail */
        assert(result->next == &list.tail);
    }

    /* Frame condition: list structure is not modified */
    assert(list.tail.prev == tail_prev_before);
    assert(list.head.next == head_next_before);
    assert(list.tail.next == tail_next_before);
    assert(list.head.prev == head_prev_before);

    /* Postcondition: list is still valid after the call */
    assert(aws_linked_list_is_valid(&list));
}
