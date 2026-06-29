#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_back_harness(void) {
    /* Allocate a linked list */
    struct aws_linked_list list;

    /* Initialize the list to establish valid structure */
    aws_linked_list_init(&list);

    /* We need a non-empty list, so add at least one node */
    struct aws_linked_list_node node;
    aws_linked_list_node_reset(&node);

    /* Push the node to the back so the list is non-empty */
    aws_linked_list_push_back(&list, &node);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save state before call to verify frame conditions */
    struct aws_linked_list_node *head_next_before = list.head.next;
    struct aws_linked_list_node *head_prev_before = list.head.prev;
    struct aws_linked_list_node *tail_next_before = list.tail.next;
    struct aws_linked_list_node *tail_prev_before = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* Postconditions (validity) */
    /* Result must not be NULL */
    assert(result != NULL);

    /* Result must equal list->tail.prev */
    assert(result == list.tail.prev);

    /* Result must not be the tail sentinel (list is non-empty) */
    assert(result != &list.tail);

    /* Result must be the head sentinel's next or a real node */
    /* For a single-element list, result should be the node we inserted */
    assert(result == &node);

    /* Postconditions (frame): list structure is not modified */
    assert(list.head.next == head_next_before);
    assert(list.head.prev == head_prev_before);
    assert(list.tail.next == tail_next_before);
    assert(list.tail.prev == tail_prev_before);

    /* List validity is preserved */
    assert(aws_linked_list_is_valid(&list));

    /* List is still non-empty */
    assert(!aws_linked_list_empty(&list));
}

void aws_linked_list_back_harness(void) {
    aws_linked_list_back_harness();
    return 0;
}
