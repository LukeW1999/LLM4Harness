#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness(void) {
    /* Allocate a node non-deterministically */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    
    /* Precondition: node must not be NULL */
    __CPROVER_assume(node != NULL);
    
    /* node->next and node->prev can be anything (non-deterministic) */
    /* They are already non-deterministic from malloc */
    
    /* Call the function under test */
    aws_linked_list_node_reset(node);
    
    /* Postcondition 1: next pointer must be NULL after reset */
    assert(node->next == NULL);
    
    /* Postcondition 2: prev pointer must be NULL after reset */
    assert(node->prev == NULL);
    
    /* Postcondition 3: The entire struct is zeroed */
    assert(AWS_IS_ZEROED(*node));
}

void aws_linked_list_node_reset_harness(void) {
    aws_linked_list_node_reset_harness();
    return 0;
}
