#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness(void) {
    /* Allocate a non-deterministic linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));

    /* Precondition: node must be non-NULL */
    __CPROVER_assume(node != NULL);

    /* node->next and node->prev can be arbitrary (non-deterministic) values,
       which malloc already provides */

    /* Call the function under verification */
    aws_linked_list_node_reset(node);

    /* Postcondition: next must be NULL after reset */
    assert(node->next == NULL);

    /* Postcondition: prev must be NULL after reset */
    assert(node->prev == NULL);
}
