#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_prev_harness() {
    /* 1. Declare a node */
    struct aws_linked_list_node node;

    /* 2. Assume the node pointer is non‑NULL (its address is always non‑NULL) */
    __CPROVER_assume(&node != NULL);

    /* 3. Save old state */
    struct aws_linked_list_node old = node;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* 5. Post‑conditions */

    /* 5a. The returned pointer must be the previous pointer stored in the node */
    assert(result == old.prev);

    /* 5b. The node itself must be unchanged */
    assert(node.prev == old.prev);
    assert(node.next == old.next);

    /* 6. No additional validity invariants are required for this pure accessor */
}
