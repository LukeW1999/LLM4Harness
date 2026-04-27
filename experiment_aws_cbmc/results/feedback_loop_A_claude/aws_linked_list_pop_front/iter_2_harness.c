#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_front_harness(void) {
    /* Declare and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save state before call */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = list.head.next->next;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    /* Postconditions */
    /* The returned node should be the old front */
    assert(result == old_front);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The new front should be the old second node */
    assert(list.head.next == old_second);

    /* The new front's prev should point back to head */
    assert(old_second->prev == &list.head);

    /* head.prev is always NULL */
    assert(list.head.prev == NULL);

    /* tail.next is always NULL */
    assert(list.tail.next == NULL);
}
