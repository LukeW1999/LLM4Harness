#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness(void) {
    /* 1. Set up the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Set up the node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state */
    /* Save the old last node (tail.prev before push) */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    bool was_empty = aws_linked_list_empty(&list);

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* The list must still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The list must not be empty after push_back */
    assert(!aws_linked_list_empty(&list));

    /* node is the new last element: tail.prev == node */
    assert(list.tail.prev == node);

    /* node's next must point to tail */
    assert(node->next == &list.tail);

    /* node's prev must point to the old tail.prev */
    assert(node->prev == old_tail_prev);

    /* The old last node's next must now point to node */
    assert(old_tail_prev->next == node);

    /* tail's next is still NULL (invariant) */
    assert(list.tail.next == NULL);

    /* head's prev is still NULL (invariant) */
    assert(list.head.prev == NULL);

    /* If the list was empty before, head.next should now point to node */
    if (was_empty) {
        assert(list.head.next == node);
    } else {
        /* head.next should be unchanged */
        assert(list.head.next == old_head_next);
    }

    /* node connectivity is valid */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
}
