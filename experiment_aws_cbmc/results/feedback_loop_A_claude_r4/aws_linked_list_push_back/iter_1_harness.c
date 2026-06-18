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

    /* 2. Declare and initialize the node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state before calling */
    /* Save the old last node (tail.prev before push) */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* Changed fields: node is now the last element (tail.prev == node) */
    assert(list.tail.prev == node);

    /* New node's next points to tail sentinel */
    assert(node->next == &list.tail);

    /* New node's prev points to what was previously the last node */
    assert(node->prev == old_tail_prev);

    /* The old last node's next now points to the new node */
    assert(old_tail_prev->next == node);

    /* Tail sentinel's prev now points to new node */
    assert(list.tail.prev == node);

    /* Head sentinel is unchanged if list was non-empty, or head.next == node if was empty */
    /* In general: head.next should still be valid */
    /* The head sentinel's next should not have changed (push_back only modifies tail side) */
    /* If the list was empty before, head.next was &list.tail, now it should be node */
    /* If the list was non-empty, head.next remains old_head_next */
    /* We verify this via the validity invariant */

    /* Sentinel invariants: head.prev == NULL, tail.next == NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* Node linkage invariants */
    /* node->next->prev == node (aws_linked_list_node_next_is_valid) */
    assert(node->next->prev == node);
    /* node->prev->next == node (aws_linked_list_node_prev_is_valid) */
    assert(node->prev->next == node);

    /* 6. Assert validity invariant holds after the call */
    assert(aws_linked_list_is_valid(&list));
}
