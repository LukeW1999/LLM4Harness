#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_remove_harness(void) {
    /* Allocate a list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* The list must be non-empty to remove a node */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Pick a node that is actually in the list (not head or tail sentinel) */
    struct aws_linked_list_node *node = list.head.next;
    /* Ensure node is not the tail sentinel */
    __CPROVER_assume(node != &list.tail);
    /* Ensure node's prev and next are non-null (should be guaranteed by valid list) */
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_node_next = node->next;
    struct aws_linked_list_node *old_node_prev = node->prev;
    struct aws_linked_list_node *old_prev_prev = node->prev->prev;
    struct aws_linked_list_node *old_next_next = node->next->next;

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* === Assertions for predecessor node === */
    /* old_node_prev->next should now point to old_node_next */
    assert(old_node_prev->next == old_node_next);
    /* old_node_prev->prev should be unchanged */
    assert(old_node_prev->prev == old_prev_prev);

    /* === Assertions for successor node === */
    /* old_node_next->prev should now point to old_node_prev */
    assert(old_node_next->prev == old_node_prev);
    /* old_node_next->next should be unchanged */
    assert(old_node_next->next == old_next_next);

    /* === List validity invariant === */
    /* The list should still be valid after removal */
    assert(aws_linked_list_is_valid(&list));
}
