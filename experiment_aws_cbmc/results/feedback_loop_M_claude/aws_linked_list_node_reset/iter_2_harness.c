#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    /* Allocate a linked list node */
    struct aws_linked_list_node node;

    /* Call function under test */
    aws_linked_list_node_reset(&node);

    /* Assert postconditions */
    assert(node.next == &node);
    assert(node.prev == &node);
}
