#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness(void) {
    /* Allocate a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Initialize with arbitrary values */
    node->next = malloc(sizeof(struct aws_linked_list_node));
    node->prev = malloc(sizeof(struct aws_linked_list_node));

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* Postconditions: node->next and node->prev should both be NULL */
    assert(node->next == NULL);
    assert(node->prev == NULL);
}
