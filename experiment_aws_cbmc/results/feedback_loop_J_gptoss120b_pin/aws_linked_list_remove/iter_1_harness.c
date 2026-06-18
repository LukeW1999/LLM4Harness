#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node and insert it so the list is non‑empty */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);
    aws_linked_list_push_back(&list, node);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    /* 3. Save old adjacency pointers */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Post‑conditions */

    /* 5a. The removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 5b. Adjacent nodes are linked together */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 5c. The node is no longer considered part of any list */
    assert(!aws_linked_list_node_is_in_list(node));

    /* 5d. The overall list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
