#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

void aws_linked_list_push_back_harness(void) {
    /* 1. Set up the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Set up the node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    bool was_empty = aws_linked_list_empty(&list);

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* The list must still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The list must not be empty after push_back */
    assert(!aws_linked_list_empty(&list));

    /* The new node must be the last element (tail.prev == node) */
    assert(list.tail.prev == node);

    /* The node's next must point to tail */
    assert(node->next == &list.tail);

    /* The node's prev must point to what was previously the last node */
    assert(node->prev == old_tail_prev);

    /* The old last node's next must now point to the new node */
    assert(old_tail_prev->next == node);

    /* The tail's prev must point to the new node */
    assert(list.tail.prev == node);

    /* The head's prev must still be NULL (never modified) */
    assert(list.head.prev == NULL);

    /* The tail's next must still be NULL (never modified) */
    assert(list.tail.next == NULL);

    /* Bidirectional linkage checks */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
}
