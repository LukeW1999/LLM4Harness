#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_swap_nodes_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Pick two distinct nodes from the list */
    struct aws_linked_list_node *a = NULL;
    struct aws_linked_list_node *b = NULL;

    size_t steps_a;
    size_t steps_b;
    __CPROVER_assume(steps_a < MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(steps_b < MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *current = list.head.next;
    for (size_t i = 0; i < steps_a && current != &list.tail; i++) {
        current = current->next;
    }
    a = current;
    __CPROVER_assume(a != &list.tail); /* ensure a is a real node */

    current = list.head.next;
    for (size_t i = 0; i < steps_b && current != &list.tail; i++) {
        current = current->next;
    }
    b = current;
    __CPROVER_assume(b != &list.tail);
    __CPROVER_assume(a != b); /* distinct nodes */

    /* Preconditions: both nodes must have valid bidirectional links */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(a));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(a));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(b));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(b));

    /* 3. Call function under test */
    aws_linked_list_swap_nodes(a, b);

    /* 4. Assert postconditions */
    assert(aws_linked_list_node_prev_is_valid(a));
    assert(aws_linked_list_node_next_is_valid(a));
    assert(aws_linked_list_node_prev_is_valid(b));
    assert(aws_linked_list_node_next_is_valid(b));

    /* 5. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
