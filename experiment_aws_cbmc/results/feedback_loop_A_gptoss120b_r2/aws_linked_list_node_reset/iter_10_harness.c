#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    /* The node may contain arbitrary pointer values before reset. */
    __CPROVER_assume(1);

    aws_linked_list_node_reset(&node);

    /* After reset both links must be NULL. */
    assert(node.prev == NULL);
    assert(node.next == NULL);
}
