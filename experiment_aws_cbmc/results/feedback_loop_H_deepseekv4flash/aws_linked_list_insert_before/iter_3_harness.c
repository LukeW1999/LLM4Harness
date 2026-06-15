#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/* Define nondet_bool if not provided */
#ifndef nondet_bool
bool nondet_bool(void) { return __CPROVER_bool; }
#endif

void aws_linked_list_insert_before_harness() {
    /* Allocate and initialize a list */
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);
    aws_linked_list_init(list);

    /* Allocate two nodes to be added */
    struct aws_linked_list_node *node1 = malloc(sizeof(*node1));
    struct aws_linked_list_node *node2 = malloc(sizeof(*node2));
    __CPROVER_assume(node1 != NULL && node2 != NULL);

    /* Push nodes onto list so that list becomes: head <-> node1 <-> node2 <-> tail */
    aws_linked_list_push_back(list, node1);
    aws_linked_list_push_back(list, node2);

    /* Validate list state */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Non-deterministically choose which node to insert before */
    struct aws_linked_list_node *before;
    if (nondet_bool()) {
        before = node1;
    } else {
        before = node2;
    }

    /* Save the previous node before the insertion point */
    struct aws_linked_list_node *old_prev = before->prev;

    /* Allocate node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);

    /* Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* Postconditions */
    assert(aws_linked_list_is_valid(list));
    assert(aws_linked_list_is_valid_deep(list));

    /* Check that to_add is correctly linked */
    assert(to_add->next == before);
    assert(to_add->prev == old_prev);
    assert(old_prev->next == to_add);
    assert(before->prev == to_add);

    /* Optional: check that before->next still points to the same node */
    if (before->next != NULL) {
        assert(before->next->prev == before);
    }
}
