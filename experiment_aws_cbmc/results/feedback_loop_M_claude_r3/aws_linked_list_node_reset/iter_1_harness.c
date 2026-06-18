#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
    /* 1. Set up a valid linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Set up a node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state before calling */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    bool was_empty = aws_linked_list_empty(&list);

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* The new node should be the last element (tail.prev == node) */
    assert(list.tail.prev == node);

    /* The node's next should point to tail */
    assert(node->next == &list.tail);

    /* The node's prev should point to the old last element */
    assert(node->prev == old_tail_prev);

    /* The old last element's next should now point to the new node */
    assert(old_tail_prev->next == node);

    /* The tail's prev should point to the new node */
    assert(list.tail.prev == node);

    /* Head should be unchanged */
    assert(list.head.prev == NULL);

    /* If the list was empty before, head.next should now point to node */
    if (was_empty) {
        assert(list.head.next == node);
    }

    /* The list should still be valid after the operation */
    assert(aws_linked_list_is_valid(&list));

    /* The list should not be empty after push_back */
    assert(!aws_linked_list_empty(&list));

    /* Node linkage validity */
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_node_next_is_valid(node));
}
