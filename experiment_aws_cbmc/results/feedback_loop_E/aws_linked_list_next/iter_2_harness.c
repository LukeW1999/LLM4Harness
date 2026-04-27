#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    /* data structure */
    struct aws_linked_list_node node;
    struct aws_linked_list_node *result;

    /* non-deterministic node setup */
    node.next = (struct aws_linked_list_node *)nondet_ptr();
    node.prev = (struct aws_linked_list_node *)nondet_ptr();

    /* assume preconditions */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    /* perform operation under verification */
    result = aws_linked_list_next(&node);

    /* assertions */
    assert(result == node.next);  // The function returns node->next

    /* unchanged fields */
    assert(node.next == (struct aws_linked_list_node *)nondet_ptr());
    assert(node.prev == (struct aws_linked_list_node *)nondet_ptr());

    /* no return value to check for success/failure, so we skip that part */

    /* validity invariants */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
    if (result != NULL) {
        assert(aws_linked_list_node_prev_is_valid(*result));
    }
}
