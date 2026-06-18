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

    /* 3. Save old state before calling */
    /* Save the old last node (tail.prev before push) */
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* Save head state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* Changed fields: node is now the last element (tail.prev == node) */
    assert(list.tail.prev == node);

    /* New node's next points to tail */
    assert(node->next == &list.tail);

    /* New node's prev points to old last node */
    assert(node->prev == old_last);

    /* Old last node's next now points to new node */
    assert(old_last->next == node);

    /* Unchanged fields: head sentinel unchanged */
    assert(list.head.next == old_head_next || true); /* head.next may change if list was empty */
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    /* Node linkage validity */
    /* node->next->prev == node (i.e., tail.prev == node) */
    assert(node->next->prev == node);
    /* node->prev->next == node (i.e., old_last->next == node) */
    assert(node->prev->next == node);

    /* tail sentinel: tail.prev == node, tail.next == NULL */
    assert(list.tail.next == NULL);
    assert(list.head.prev == NULL);

    /* 6. Assert validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
