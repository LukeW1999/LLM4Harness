#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_front_harness(void) {
    /* Stack-allocate a linked list */
    struct aws_linked_list list;

    /* Initialize the list to a valid state */
    aws_linked_list_init(&list);

    /* Allocate at least one node to make the list non-empty */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Push the node to the front so the list is non-empty */
    aws_linked_list_push_front(&list, node);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save state before call for frame condition checks */
    struct aws_linked_list_node *head_next_before = list.head.next;
    struct aws_linked_list_node *head_prev_before = list.head.prev;
    struct aws_linked_list_node *tail_next_before = list.tail.next;
    struct aws_linked_list_node *tail_prev_before = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* Postconditions (validity) */
    assert(result != NULL);
    assert(result == list.head.next);
    assert(result != &list.tail);

    /* Postconditions (frame): list structure is not modified */
    assert(list.head.next == head_next_before);
    assert(list.head.prev == head_prev_before);
    assert(list.tail.next == tail_next_before);
    assert(list.tail.prev == tail_prev_before);

    /* List remains valid after the call */
    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));
}
