#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_node_reset_harness(void) {
    /* 1. Allocate a linked list node */
    struct aws_linked_list_node *node;
    ensure_linked_list_node_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(node != NULL);

    /* 2. Save the original links */
    struct aws_linked_list_node *orig_next = node->next;
    struct aws_linked_list_node *orig_prev = node->prev;

    /* 3. Call the function under test */
    aws_linked_list_node_reset(node);

    /* 4. Post‑condition: the node must point to itself */
    assert(node->next == node);
    assert(node->prev == node);

    /* 5. Distinguish the two possible original states */
    if (orig_next == node && orig_prev == node) {
        /* The node was already reset – nothing else should have changed */
        assert(node->next == node);
        assert(node->prev == node);
    } else {
        /* The node was previously linked – after reset it must be isolated */
        /* No additional constraints on the former neighbours are required,
           but the node itself must be self‑referential as asserted above. */
        assert(node->next == node);
        assert(node->prev == node);
    }

    /* 6. The node pointer itself must remain non‑NULL */
    assert(node != NULL);
}
