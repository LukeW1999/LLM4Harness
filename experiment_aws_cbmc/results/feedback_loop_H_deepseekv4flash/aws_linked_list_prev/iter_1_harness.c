#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness() {
    /* Declare and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure list is not empty so we can pick a node */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old list state (immutability check) */
    struct aws_linked_list old_list = list;

    /* Pick the first real node in the list */
    struct aws_linked_list_node *node = list.head.next;

    /* Call the function under test */
    struct aws_linked_list_node *prev = aws_linked_list_prev(node);

    /* Postcondition: prev equals node->prev */
    assert(prev == node->prev);

    /* No modifications to the list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* List validity preserved */
    assert(aws_linked_list_is_valid(&list));
}
