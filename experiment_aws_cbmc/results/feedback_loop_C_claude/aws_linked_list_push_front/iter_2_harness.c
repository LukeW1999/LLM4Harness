#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_linked_list_push_front_harness(void) {
    /* Allocate and initialize a valid linked list */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);
    
    /* Initialize the list properly */
    aws_linked_list_init(list);
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Allocate a new node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list->head.next;
    bool was_empty = aws_linked_list_empty(list);

    /* Call the function under test */
    aws_linked_list_push_front(list, node);

    /* Postcondition: list->head.next must be node */
    assert(list->head.next == node);

    /* Postcondition: node->prev must be &list->head */
    assert(node->prev == &list->head);

    /* Postcondition: node->next must be old head.next */
    assert(node->next == old_head_next);

    /* Postcondition: list->head.prev must remain NULL */
    assert(list->head.prev == NULL);

    /* Postcondition: list->tail.next must remain NULL */
    assert(list->tail.next == NULL);

    /* Postcondition: tail.prev changes to node if list was empty */
    if (was_empty) {
        assert(list->tail.prev == node);
    } else {
        assert(list->tail.prev != NULL);
    }

    /* Postcondition: validity invariants */
    assert(aws_linked_list_is_valid(list));

    /* Postcondition: list must not be empty after push_front */
    assert(!aws_linked_list_empty(list));
}
