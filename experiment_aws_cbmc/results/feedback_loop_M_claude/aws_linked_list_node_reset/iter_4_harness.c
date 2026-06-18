#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    /* Allocate a linked list node */
    struct aws_linked_list_node node;

    /* Initialize with non-deterministic values */
    node.next = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    node.prev = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));

    /* Call function under test */
    aws_linked_list_node_reset(&node);

    /* Assert postconditions - node pointers should be NULL after reset */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
