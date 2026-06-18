#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    /* 1. Set up a node to reset */
    struct aws_linked_list_node node;

    /* 2. Call function under test */
    aws_linked_list_node_reset(&node);

    /* 3. Assert postconditions */
    /* After reset, next and prev should be NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
