#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node dummy;
    
    /* Non-deterministically initialize the node to simulate any prior state */
    if (nondet_bool()) {
        node.next = &dummy;
    } else {
        node.next = NULL;
    }
    
    if (nondet_bool()) {
        node.prev = &dummy;
    } else {
        node.prev = NULL;
    }
    
    /* Call the function under test */
    aws_linked_list_node_reset(&node);
    
    /* Postconditions: next and prev pointers are set to NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
