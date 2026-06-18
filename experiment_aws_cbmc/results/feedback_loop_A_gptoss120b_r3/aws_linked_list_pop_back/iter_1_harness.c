#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_node_reset_harness() {
    /* Allocate a node (non-deterministic contents) */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* Save old state for potential immutability checks (none needed here) */
    struct aws_linked_list_node old = *node;

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* Postconditions: the node must be zeroed (next and prev set to NULL) */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* Unchanged fields: there are no other fields in the struct, so nothing else to assert */

    /* Validity invariants: after reset the node is not linked, which satisfies the
       node‑in‑list predicate (implicitly true because both links are NULL) */
    assert(aws_linked_list_node_is_in_list(node) == false);

    free(node);
}
