#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_nodes_harness() {
    /* Build a valid linked list with two interior nodes */
    struct aws_linked_list list;
    struct aws_linked_list_node node1;
    struct aws_linked_list_node node2;

    /* Initialize list as empty */
    aws_linked_list_init(&list);

    /* Insert node1 after head */
    node1.next = list.head.next;          /* = &list.tail */
    node1.prev = &list.head;
    list.head.next = &node1;
    list.tail.prev = &node1;

    /* Insert node2 after node1 */
    node2.next = node1.next;              /* = &list.tail */
    node2.prev = &node1;
    node1.next = &node2;
    list.tail.prev = &node2;

    /* List is now: head <-> node1 <-> node2 <-> tail */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node1));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node1));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node2));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node2));

    /* Choose two nodes to swap (here we fix them for simplicity) */
    struct aws_linked_list_node *a = &node1;
    struct aws_linked_list_node *b = &node2;

    /* Save old state for postcondition checks */
    struct aws_linked_list_node old_a = *a;
    struct aws_linked_list_node old_b = *b;
    struct aws_linked_list old_list = list;

    /* Call the function under test */
    aws_linked_list_swap_nodes(a, b);

    /* Postcondition 1: List remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 2: Both nodes remain valid */
    assert(aws_linked_list_node_prev_is_valid(a));
    assert(aws_linked_list_node_next_is_valid(a));
    assert(aws_linked_list_node_prev_is_valid(b));
    assert(aws_linked_list_node_next_is_valid(b));

    /* Postcondition 3: Nodes are swapped (if distinct) */
    if (a != b) {
        assert(a->prev == old_b.prev);
        assert(a->next == old_b.next);
        assert(b->prev == old_a.prev);
        assert(b->next == old_a.next);

        /* Neighbor pointers are consistent with the swap */
        assert(old_a.prev->next == b);
        assert(old_a.next->prev == b);
        assert(old_b.prev->next == a);
        assert(old_b.next->prev == a);
    } else {
        /* No change when a == b */
        assert(a->prev == old_a.prev);
        assert(a->next == old_a.next);
    }

    /* Postcondition 4: Unchanged fields from the list (head and tail pointers unchanged) */
    assert(list.head.next == old_list.head.next || list.head.next == &node2); /* ordering may change */
    assert(list.tail.prev == old_list.tail.prev || list.tail.prev == &node1);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
