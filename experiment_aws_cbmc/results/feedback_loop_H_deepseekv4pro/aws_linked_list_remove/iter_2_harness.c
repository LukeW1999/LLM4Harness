#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* nondeterministic boolean, used to select a node from the list */
bool nondet_bool();

void aws_linked_list_remove_harness() {
    /* Create a bounded, well-formed linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* Pick an arbitrary non-sentinel node from the list */
    struct aws_linked_list_node *node = list.head.next;
    while (node != &list.tail && nondet_bool()) {
        node = node->next;
    }
    __CPROVER_assume(node != &list.head);
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(node != NULL);

    /* Strengthen: the node is truly part of the list and has valid links */
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);

    /* Save neighbours for postcondition checks */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* Postconditions */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);
}
