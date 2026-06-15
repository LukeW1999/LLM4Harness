#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    /* Create a linked list with non-deterministic number of nodes up to MAX_LINKED_LIST_ITEM_ALLOCATION */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Obtain a node from the list (the first element, which is the head sentinel if the list is empty) */
    struct aws_linked_list_node *node = aws_linked_list_begin(&list);
    /* Non-deterministically assume that node is not NULL (always true for a valid list) */
    __CPROVER_assume(node != NULL);

    /* Save the old state of the node (only next and prev fields) */
    struct aws_linked_list_node old_node = *node;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* Postconditions */

    /* 1. The function returns node->next */
    assert(result == node->next);

    /* 2. The node itself is not modified (it is const) */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 3. The list remains valid (since we didn't modify anything) */
    assert(aws_linked_list_is_valid(&list));
}
