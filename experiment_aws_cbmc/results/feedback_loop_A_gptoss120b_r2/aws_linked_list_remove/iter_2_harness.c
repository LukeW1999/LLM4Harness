#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_remove_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create a node and insert it into the list so it is removable */
    struct aws_linked_list_node node;
    aws_linked_list_node_reset(&node);
    aws_linked_list_push_back(&list, &node);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    /* 3. Save old state before the call */
    struct aws_linked_list_node *old_prev = node.prev;
    struct aws_linked_list_node *old_next = node.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list old_list = list;   /* whole list copy for unchanged checks */

    /* 4. Call the function under test */
    aws_linked_list_remove(&node);

    /* 5. Assert changed fields */
    /* node's pointers must be reset */
    assert(node.prev == NULL);
    assert(node.next == NULL);

    /* surrounding list links must be updated */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 6. Assert unchanged or appropriately updated sentinel links */
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.head.next == old_head_next || list.head.next == old_next);
    assert(list.tail.prev == old_tail_prev || list.tail.prev == old_prev);

    /* 7. Assert validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
