#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    
    /* Precondition: node != NULL is satisfied by passing the address of a local variable */
    aws_linked_list_node_reset(&node);
    
    /* Postcondition: node's next and prev pointers are set to NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
