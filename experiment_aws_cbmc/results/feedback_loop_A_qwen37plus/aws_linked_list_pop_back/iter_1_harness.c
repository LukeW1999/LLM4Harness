#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;
    
    aws_linked_list_node_reset(&node);
    
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
