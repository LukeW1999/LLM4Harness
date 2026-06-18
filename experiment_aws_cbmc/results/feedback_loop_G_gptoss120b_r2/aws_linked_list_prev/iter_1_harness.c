#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node from the list (could be head.next, which is valid even for empty list) */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != NULL); /* node must be a valid pointer */

    /* Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_node = node;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 4. Postconditions */
    /* The function should return the previous pointer of the input node */
    assert(result == old_prev);

    /* No fields of the list should have changed */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* The node itself should remain unchanged */
    assert(node->prev == old_prev);
    assert(node->next == old_next);

    /* 5. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
