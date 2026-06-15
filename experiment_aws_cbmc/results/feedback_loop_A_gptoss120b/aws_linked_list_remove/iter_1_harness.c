#include <stdlib.h>
#include <assert.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_remove_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node and insert it into the list */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);
    aws_linked_list_push_back(&list, node);

    /* 3. Save old state before the call */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Post‑condition asserts */

    /* node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* neighboring nodes are linked together */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* the node is no longer considered part of a list */
    assert(!aws_linked_list_node_is_in_list(node));

    /* list structural invariants are preserved */
    assert(aws_linked_list_is_valid(&list));

    /* fields of the list that are not supposed to change remain unchanged */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
}
