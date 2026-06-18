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

    /* Changed: node is now the last element (tail.prev == node) */
    assert(list.tail.prev == node);

    /* Changed: node's next points to tail sentinel */
    assert(node->next == &list.tail);

    /* Changed: node's prev points to the old last node */
    assert(node->prev == old_tail_prev);

    /* Changed: old last node's next now points to node */
    assert(old_tail_prev->next == node);

    /* Unchanged: head sentinel's prev is still NULL */
    assert(list.head.prev == NULL);

    /* Unchanged: tail sentinel's next is still NULL */
    assert(list.tail.next == NULL);

    /* Validity invariant: list structure is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* Node linkage validity */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(&list.tail));
}
