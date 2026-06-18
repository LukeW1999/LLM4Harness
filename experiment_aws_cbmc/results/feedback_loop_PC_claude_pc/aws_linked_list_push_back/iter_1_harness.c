#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare the node to push back */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state BEFORE calling */
    /* Save the old last node (the node currently before tail) */
    struct aws_linked_list_node *old_last = list.tail.prev;
    /* Save head sentinel state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    /* Save head.prev (should be NULL per validity) */
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    /* Save tail.next (should be NULL per validity) */
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* Changed fields: node is now the last element before tail */
    assert(list.tail.prev == node);

    /* node->next must point to tail */
    assert(node->next == &list.tail);

    /* node->prev must point to old last node */
    assert(node->prev == old_last);

    /* old_last->next must now point to node */
    assert(old_last->next == node);

    /* Sentinel invariants: head.prev and tail.next must remain NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* head.next should be unchanged (we pushed to back, not front) */
    /* Only unchanged if list was non-empty before; if list was empty,
       head.next was &list.tail before, now head.next should be node */
    /* We verify the linkage is consistent via validity */

    /* node->next->prev == node (aws_linked_list_node_next_is_valid) */
    assert(node->next->prev == node);

    /* node->prev->next == node (aws_linked_list_node_prev_is_valid) */
    assert(node->prev->next == node);

    /* tail.prev->next == &list.tail (aws_linked_list_node_prev_is_valid for tail) */
    assert(list.tail.prev->next == &list.tail);

    /* head.next->prev == &list.head (aws_linked_list_node_next_is_valid for head) */
    assert(list.head.next->prev == &list.head);

    /* Unchanged: tail.next remains NULL */
    assert(list.tail.next == old_tail_next);
    assert(list.tail.next == NULL);

    /* Unchanged: head.prev remains NULL */
    assert(list.head.prev == old_head_prev);
    assert(list.head.prev == NULL);

    /* 6. Assert validity invariant holds after the call */
    assert(aws_linked_list_is_valid(&list));
}
