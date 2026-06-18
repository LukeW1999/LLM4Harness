#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness(void) {
    /* 1. Set up the node */
    struct aws_linked_list_node node;
    node.next = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    node.prev = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(node.prev != NULL);

    /* 2. Call function under test */
    aws_linked_list_node_reset(&node);

    /* 3. Assert postconditions */
    /* After reset, next and prev should point to the node itself */
    assert(node.next == &node);
    assert(node.prev == &node);
}
