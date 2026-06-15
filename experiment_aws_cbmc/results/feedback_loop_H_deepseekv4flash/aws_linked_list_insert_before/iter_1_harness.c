#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness() {
    /* Setup: create an empty list and allocate it (initializes head and tail) */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, 2);

    /* Create two regular nodes and one node to insert */
    struct aws_linked_list_node node1, node2, to_add;
    node1.next = node1.prev = NULL;
    node2.next = node2.prev = NULL;
    to_add.next = to_add.prev = NULL;

    /* Add node1 and node2 to the list (so the list is head <-> node1 <-> node2 <-> tail) */
    aws_linked_list_push_back(&list, &node1);
    aws_linked_list_push_back(&list, &node2);

    /* Ensure the list is valid initially */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Non‑deterministically choose which node to insert before */
    struct aws_linked_list_node *before;
    if (nondet_bool()) {
        before = &node1;
    } else {
        before = &node2;
    }

    /* Save the node that will be before the inserted node */
    struct aws_linked_list_node *old_prev = before->prev;

    /* Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* Post‑condition 1: the list remains valid (immediate checks) */
    assert(aws_linked_list_is_valid(&list));
    /* Optional: also check deep validity (full connectivity) */
    assert(aws_linked_list_is_valid_deep(&list));

    /* Post‑condition 2: to_add is correctly linked */
    assert(to_add.next == before);
    assert(to_add.prev == old_prev);
    assert(old_prev->next == &to_add);
    assert(before->prev == &to_add);

    /* Post‑condition 3: the node after before (if any) still points back to before */
    if (before->next != NULL) {
        assert(before->next->prev == before);
    }

    /* No other fields to check – the list struct contains only head and tail,
     * both of which have changed links, so no immutable field exists. */
}
