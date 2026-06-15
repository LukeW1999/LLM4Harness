#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_nodes_harness() {
    /* Non-deterministic data */
    struct aws_linked_list_node a;
    struct aws_linked_list_node b;

    /* We need to ensure that a and b are valid nodes in a list.
     * For simplicity, we assume they are part of a valid linked list.
     * We also need to ensure that a and b are not the same node initially,
     * but the function handles that case.
     */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&a));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&a));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&b));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&b));

    /* Save old state */
    struct aws_linked_list_node old_a = a;
    struct aws_linked_list_node old_b = b;

    /* Call the function */
    aws_linked_list_swap_nodes(&a, &b);

    /* Postconditions */
    if (&a == &b) {
        /* If a and b are the same node, nothing changes */
        assert(a.next == old_a.next);
        assert(a.prev == old_a.prev);
    } else {
        /* The nodes are swapped: a now has b's old next/prev, b has a's old next/prev */
        assert(a.next == old_b.next);
        assert(a.prev == old_b.prev);
        assert(b.next == old_a.next);
        assert(b.prev == old_a.prev);
    }

    /* Validity invariants */
    assert(aws_linked_list_node_prev_is_valid(&a));
    assert(aws_linked_list_node_next_is_valid(&a));
    assert(aws_linked_list_node_prev_is_valid(&b));
    assert(aws_linked_list_node_next_is_valid(&b));
}
