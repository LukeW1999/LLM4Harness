#include <aws/common/linked_list.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

/* Compute the number of user nodes in the list (excluding sentinels). */
static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        len++;
        cur = cur->next;
    }
    return len;
}

void harness(void) {
    /* Allocate and initialise an empty list. */
    struct aws_linked_list list;
    list.head.prev = NULL;
    list.tail.next = NULL;
    list.head.next = &list.tail;
    list.tail.prev = &list.head;

    /* Allocate a node to be pushed. */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    node->next = NULL;
    node->prev = NULL;

    /* Record pre‑state. */
    struct aws_linked_list list_pre = list;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    size_t len_before = list_length(&list);

    /* Call the function under test. */
    aws_linked_list_push_back(&list, node);

    /* 1. Validity predicates. */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == node);
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));

    /* 2. Length invariant (list length increased by exactly one). */
    assert(list_length(&list) == len_before + 1);

    /* 3. Frame conditions. */
    /* Sentinels unchanged except for the links that must be updated. */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next == list_pre.head.next);
    assert(list.tail.prev == node);
    /* The node that previously preceded the tail is now linked to the new node. */
    assert(old_tail_prev->next == node);
    /* New node links are correct. */
    assert(node->next == &list.tail);
    assert(node->prev == old_tail_prev);
}
