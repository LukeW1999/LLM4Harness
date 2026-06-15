#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_node_reset_harness(void) {
    /* Allocate a linked list node */
    struct aws_linked_list_node *node;
    ensure_linked_list_node_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(node != NULL);

    /* Initialise the node to a valid linked‑list state (self‑referential) */
    node->next = node;
    node->prev = node;

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* Post‑condition: the node must point to itself */
    assert(node->next == node);
    assert(node->prev == node);
}
