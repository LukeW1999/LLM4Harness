#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_node_reset_harness(void) {
    /* 1. Set up the node - use a stack-allocated node */
    struct aws_linked_list_node node;

    /* Leave next and prev uninitialized (nondet) - CBMC will use symbolic values */

    /* 2. Call the function under test */
    aws_linked_list_node_reset(&node);

    /* 3. Assert postconditions */
    /* After reset, next and prev should be NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
