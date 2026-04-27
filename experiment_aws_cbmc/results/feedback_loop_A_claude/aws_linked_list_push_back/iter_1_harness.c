#include <assert.h>
#include <stdlib.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

/* Validity predicates used in the proof */
bool aws_linked_list_is_valid(const struct aws_linked_list *list) {
    /* head.prev == NULL, tail.next == NULL, and head <-> tail linkage is intact */
    return list != NULL &&
           list->head.prev == NULL &&
           list->tail.next == NULL &&
           list->head.next != NULL &&
           list->tail.prev != NULL &&
           list->head.next->prev == &list->head &&
           list->tail.prev->next == &list->tail;
}

bool aws_linked_list_node_next_is_valid(const struct aws_linked_list_node *node) {
    return node != NULL && node->next != NULL && node->next->prev == node;
}

bool aws_linked_list_node_prev_is_valid(const struct aws_linked_list_node *node) {
    return node != NULL && node->prev != NULL && node->prev->next == node;
}

void aws_linked_list_push_back_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare the node to push back */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state before calling */
    /* Save the old last node (the node currently before tail) */
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* Save head state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev; /* should be NULL */

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* Changed fields: node is now the last element before tail */
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);
    assert(node->prev == old_last);
    assert(old_last->next == node);

    /* Sentinel invariants: head.prev must remain NULL, tail.next must remain NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* head.next should be unchanged (we pushed to back, not front) */
    /* Only unchanged if the list was non-empty before; if it was empty,
       head.next was &list.tail and now head.next should be node */
    /* Actually: if old_last == &list.head (empty list), then head.next == node now */
    /* If old_last != &list.head (non-empty list), then head.next == old_head_next */
    if (old_last == &list.head) {
        /* List was empty: head.next should now point to node */
        assert(list.head.next == node);
    } else {
        /* List was non-empty: head.next unchanged */
        assert(list.head.next == old_head_next);
    }

    /* Node linkage validity */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(&list.tail));

    /* 6. Assert validity invariant holds after the call */
    assert(aws_linked_list_is_valid(&list));
}

int main(void) {
    aws_linked_list_push_back_harness();
    return 0;
}
