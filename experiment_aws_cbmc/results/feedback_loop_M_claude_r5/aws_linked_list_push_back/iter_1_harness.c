#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
    /* 1. Set up the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before the call */
    /* Save the old last node (tail.prev) before push_back */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 3. Set up the node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* The new node should be the last element (tail.prev == node) */
    assert(list.tail.prev == node);

    /* The node's next should point to tail */
    assert(node->next == &list.tail);

    /* The node's prev should point to the old last node */
    assert(node->prev == old_tail_prev);

    /* The old last node's next should now point to the new node */
    assert(old_tail_prev->next == node);

    /* The tail's prev should be the new node */
    assert(list.tail.prev == node);

    /* Sentinel invariants: head.prev == NULL, tail.next == NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* Node linkage validity */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(&list.tail));

    /* Overall list validity */
    assert(aws_linked_list_is_valid(&list));
}
