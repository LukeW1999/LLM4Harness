#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node from the list (could be head.next, which is valid even for empty list) */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != NULL); /* node must be a valid pointer */

    /* 3. Save old state before calling */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_node_prev = node->prev;
    struct aws_linked_list_node *old_node_next = node->next;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Postconditions */
    /* The function should return the previous pointer unchanged */
    assert(result == old_node_prev);

    /* No fields of the node should be modified */
    assert(node->prev == old_node_prev);
    assert(node->next == old_node_next);

    /* The list structure must remain unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 6. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
}
