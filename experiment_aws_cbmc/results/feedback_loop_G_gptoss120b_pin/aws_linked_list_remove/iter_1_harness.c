#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness() {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);

    /* 2. Allocate a node to be removed and add it to the list */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);
    aws_linked_list_push_back(&list, node);

    /* 3. Assume the list is valid and the node is in the list before removal */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    /* 4. Save old adjacency pointers */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 5. Call the function under test */
    aws_linked_list_remove(node);

    /* 6. Post‑conditions */

    /* The removed node's pointers are cleared */
    assert(node->prev == NULL);
    assert(node->next == NULL);

    /* The surrounding nodes now point to each other */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* The node is no longer considered part of any list */
    assert(!aws_linked_list_node_is_in_list(node));

    /* The list remains a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));
}
