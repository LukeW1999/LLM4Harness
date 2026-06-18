#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_next_harness(void) {
    /* Allocate a node and make it a valid linked‑list node (self‑referential) */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    node->next = node;
    node->prev = node;

    /* Snapshot the original next pointer */
    struct aws_linked_list_node *orig_next = node->next;

    /* Call the function under verification */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* Post‑conditions */
    assert(result == orig_next);
    assert(node->next == orig_next);
}
