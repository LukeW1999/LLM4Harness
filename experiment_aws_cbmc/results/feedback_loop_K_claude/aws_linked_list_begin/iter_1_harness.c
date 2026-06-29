#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_begin_harness(void) {
    /* Allocate a linked list */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));

    /* Precondition: list must be non-NULL */
    __CPROVER_assume(list != NULL);

    /* Initialize the list to ensure it is valid */
    aws_linked_list_init(list);

    /* Optionally add a node to cover non-empty case */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    bool add_node;
    if (add_node) {
        aws_linked_list_push_back(list, node);
    }

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Save state before call for frame condition checks */
    struct aws_linked_list_node *head_next_before = list->head.next;
    struct aws_linked_list_node *head_prev_before = list->head.prev;
    struct aws_linked_list_node *tail_next_before = list->tail.next;
    struct aws_linked_list_node *tail_prev_before = list->tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(list);

    /* Postcondition: return value is non-NULL */
    assert(result != NULL);

    /* Postcondition: return value equals list->head.next */
    assert(result == head_next_before);

    /* Postcondition: frame conditions - list is unchanged */
    assert(list->head.next == head_next_before);
    assert(list->head.prev == head_prev_before);
    assert(list->tail.next == tail_next_before);
    assert(list->tail.prev == tail_prev_before);

    /* Postcondition: list is still valid after the call */
    assert(aws_linked_list_is_valid(list));

    /* Postcondition: if list is empty, result points to tail */
    if (aws_linked_list_empty(list)) {
        assert(result == &list->tail);
    }

    /* Postcondition: if list is non-empty, result is not the tail */
    if (!aws_linked_list_empty(list)) {
        assert(result != &list->tail);
    }
}
