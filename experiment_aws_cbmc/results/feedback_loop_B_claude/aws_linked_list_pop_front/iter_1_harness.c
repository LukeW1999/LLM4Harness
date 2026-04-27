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

    /* The new node should be the last element (tail.prev == node) */
    assert(list.tail.prev == node);

    /* node's next should point to tail */
    assert(node->next == &list.tail);

    /* node's prev should point to what was previously the last element */
    assert(node->prev == old_tail_prev);

    /* The old last element's next should now point to node */
    assert(old_tail_prev->next == node);

    /* head.next should still point to the first element */
    /* If list was empty before, head.next should now point to node */
    if (was_empty) {
        assert(list.head.next == node);
    }

    /* head.prev should still be NULL */
    assert(list.head.prev == NULL);

    /* tail.next should still be NULL */
    assert(list.tail.next == NULL);

    /* The list should no longer be empty */
    assert(!aws_linked_list_empty(&list));

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* node should be in the list (both prev and next are valid) */
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_node_next_is_valid(node));
}
