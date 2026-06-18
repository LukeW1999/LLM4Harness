#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    /* The fields of `node` are nondeterministic by default. */

    aws_linked_list_node_reset(&node);

    assert(node.prev == NULL);
    assert(node.next == NULL);
}
