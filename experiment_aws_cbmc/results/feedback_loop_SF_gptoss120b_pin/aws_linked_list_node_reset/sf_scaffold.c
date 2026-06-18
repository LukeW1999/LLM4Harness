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
}
