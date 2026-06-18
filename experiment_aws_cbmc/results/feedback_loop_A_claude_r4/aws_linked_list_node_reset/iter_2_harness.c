#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness(void) {
    /* 1. Set up the node */
    struct aws_linked_list_node node;
    node.next = malloc(sizeof(struct aws_linked_list_node));
    node.prev = malloc(sizeof(struct aws_linked_list_node));

    /* 2. Call function under test */
    aws_linked_list_node_reset(&node);

    /* 3. Assert postconditions */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
