#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_linked_list_push_front_harness() {
    /* Allocate list and node dynamically to avoid aliasing issues */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));

    /* Initialize list as empty */
    aws_linked_list_init(list);

    /* Assume the list is valid */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Initialize the new node as not part of any list */
    to_add->next = NULL;
    to_add->prev = NULL;

    /* Call the function under test */
    aws_linked_list_push_front(list, to_add);

    /* Postcondition 1: List remains valid */
    assert(aws_linked_list_is_valid(list));

    /* Postcondition 2: The new node is at the front */
    assert(list->head.next == to_add);
    assert(to_add->prev == &list->head);

    /* Postcondition 3: The new node points to the tail (since list was empty) */
    assert(to_add->next == &list->tail);

    /* Postcondition 4: Tail's prev now points to the new node */
    assert(list->tail.prev == to_add);

    /* Postcondition 5: Sentinel pointers remain NULL */
    assert(list->head.prev == NULL);
    assert(list->tail.next == NULL);

    /* Postcondition 6: The new node is valid */
    assert(aws_linked_list_node_prev_is_valid(to_add));
    assert(aws_linked_list_node_next_is_valid(to_add));
}
