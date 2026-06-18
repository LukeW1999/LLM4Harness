#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node new_node;

    /* Initialize a valid, bounded linked list */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_bounded(&list, MAX_LINKED_LIST_ITEM_ALLOCATION));
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state for postcondition checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under verification */
    aws_linked_list_push_back(&list, &new_node);

    /* Postcondition 1: List remains valid after insertion */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 2: new_node is now the last element (before tail sentinel) */
    assert(list.tail.prev == &new_node);

    /* Postcondition 3: new_node's next is the tail sentinel */
    assert(new_node.next == &list.tail);

    /* Postcondition 4: new_node's prev is the old last node */
    assert(new_node.prev == old_tail_prev);

    /* Postcondition 5: old last node's next now points to new_node */
    assert(old_tail_prev->next == &new_node);

    /* Sentinel invariants: head.prev and tail.next must remain NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
