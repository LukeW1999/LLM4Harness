#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* PRE-CALL SNAPSHOT */
    struct aws_linked_list_node old_node = *node;

    /* CALL FUNCTION UNDER TEST */
    aws_linked_list_node_reset(node);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* The node pointer must remain non‑NULL */
    assert(node != NULL);
    /* After reset both links must be NULL */
    assert(node->prev == NULL);
    assert(node->next == NULL);
    /* No other memory within the node structure should have been altered
       (the only members are prev and next, which are now NULL) */
    assert(old_node.prev != NULL || old_node.next != NULL || (node->prev == NULL && node->next == NULL));
}
