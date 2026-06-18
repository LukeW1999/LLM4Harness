#include <assert.h>
#include <stdlib.h>
#include <aws/common/allocator.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();
    (void)alloc; /* allocator not used directly in this harness */

    struct aws_linked_list_node node;

    /* Initialize the node fields to a known state */
    node.next = &node;
    node.prev = &node;

    aws_linked_list_node_reset(&node);

    assert(node.next == &node);
    assert(node.prev == &node);
}
