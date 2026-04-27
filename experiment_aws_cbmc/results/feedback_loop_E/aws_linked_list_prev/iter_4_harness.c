#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness() {
    /* data structure */
    struct aws_linked_list_node node;
    struct aws_linked_list_node *prev_node;

    /* non-deterministic allocation and initialization */
    prev_node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);

    /* setup node pointers */
    node.prev = prev_node;
    node.next = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node.next != NULL);

    /* Add assumptions to satisfy preconditions */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* assertions */
    assert(result == prev_node);
}
