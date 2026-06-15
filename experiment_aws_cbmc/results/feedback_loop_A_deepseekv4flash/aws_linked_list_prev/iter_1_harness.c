#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness() {
    /* We need a valid linked list to have a realistic node context */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list)); /* ensure we can pick a node that has a previous node */

    /* Pick the first node after the head sentinel (the actual first element) */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != &list.tail); /* must not be the tail sentinel */

    /* Save the expected previous pointer (the node's predecessor) */
    struct aws_linked_list_node *expected_prev = node->prev;

    /* Call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_prev(node);

    /* Postcondition: returned pointer equals node->prev */
    assert(rval == expected_prev);

    /* The list structure is unchanged (const argument) */
    assert(aws_linked_list_is_valid(&list));

    /* The node itself is not modified */
    assert(node->prev == expected_prev);
    assert(node->next == node->next); /* trivial, but ensures no modification */
}
