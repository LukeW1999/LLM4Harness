#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* Initialize node as a valid list node (self‑referential) */
    node->next = node;
    node->prev = node;

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* After reset, the node should have NULL pointers */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    free(node);
}
