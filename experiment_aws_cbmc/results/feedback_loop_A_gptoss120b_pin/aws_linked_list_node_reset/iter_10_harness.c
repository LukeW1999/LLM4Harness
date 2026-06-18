#include <assert.h>
#include <stddef.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    /* The fields of 'node' are nondeterministic at this point. */
    aws_linked_list_node_reset(&node);
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
