#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_nodes_harness() {
    /* Create a linked list and allocate its nodes dynamically */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Choose two distinct nodes from the list */
    struct aws_linked_list_node *a = list.head.next;
    /* Ensure a is not the tail sentinel (list has at least one actual node) */
    __CPROVER_assume(a != &list.tail);
    struct aws_linked_list_node *b = a->next;
    /* Ensure there are at least two nodes (b is not tail and different from a) */
    __CPROVER_assume(b != &list.tail && a != b);

    /* Assume both nodes are valid (prev and next pointers are bidirectional) */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(a));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(a));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(b));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(b));

    /* Call the function under test */
    aws_linked_list_swap_nodes(a, b);

    /* Assert postconditions */
    /* 1. The entire list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* 2. Both swapped nodes remain valid */
    assert(aws_linked_list_node_prev_is_valid(a));
    assert(aws_linked_list_node_next_is_valid(a));
    assert(aws_linked_list_node_prev_is_valid(b));
    assert(aws_linked_list_node_next_is_valid(b));

    /* 3. The list is not empty after the swap (it was nonempty before) */
    assert(!aws_linked_list_empty(&list));

    /* 4. Sentinel pointers (head and tail) are still connected to nodes */
    /*    head.next must point to some node (not NULL) */
    assert(list.head.next != NULL);
    /*    tail.prev must point to some node (not NULL) */
    assert(list.tail.prev != NULL);
}
