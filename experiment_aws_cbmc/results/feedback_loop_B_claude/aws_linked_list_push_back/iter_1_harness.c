#include <assert.h>
#include <stdlib.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

/* Validity predicates used inline */
static bool aws_linked_list_node_next_is_valid(const struct aws_linked_list_node *node) {
    return node && node->next && node->next->prev == node;
}

static bool aws_linked_list_node_prev_is_valid(const struct aws_linked_list_node *node) {
    return node && node->prev && node->prev->next == node;
}

static bool aws_linked_list_is_valid(const struct aws_linked_list *list) {
    return list &&
           list->head.prev == NULL &&
           list->tail.next == NULL &&
           list->head.next != NULL &&
           list->tail.prev != NULL &&
           list->head.next->prev == &list->head &&
           list->tail.prev->next == &list->tail;
}

void aws_linked_list_push_back_harness(void) {
    /* 1. Set up the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Set up the node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state before the call */
    /* Save the old last node (the one currently before tail) */
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* Save head state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* The list must still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* node is now the last element: tail.prev == node */
    assert(list.tail.prev == node);

    /* node->next must point to tail */
    assert(node->next == &list.tail);

    /* node->prev must point to old_last */
    assert(node->prev == old_last);

    /* old_last->next must now point to node */
    assert(old_last->next == node);

    /* tail.next is still NULL (sentinel invariant) */
    assert(list.tail.next == NULL);

    /* head.prev is still NULL (sentinel invariant) */
    assert(list.head.prev == NULL);

    /* head.next is unchanged (we pushed to back, not front) */
    assert(list.head.next == old_head_next);

    /* Node linkage validity */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(&list.tail));
}
