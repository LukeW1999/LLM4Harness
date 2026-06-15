#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node1, node2;
    int num_nodes;

    /* Non-deterministically choose 1 or 2 nodes */
    __CPROVER_assume(num_nodes == 1 || num_nodes == 2);

    if (num_nodes == 1) {
        // List with exactly one node
        list.head.prev = NULL;
        list.head.next = &node1;
        list.tail.prev = &node1;
        list.tail.next = NULL;
        node1.next = &list.tail;
        node1.prev = &list.head;
    } else {
        // List with two nodes
        list.head.prev = NULL;
        list.head.next = &node1;
        list.tail.prev = &node2;
        list.tail.next = NULL;
        node1.next = &node2;
        node1.prev = &list.head;
        node2.next = &list.tail;
        node2.prev = &node1;
    }

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    bool old_list_has_one_node = (old_head_next == old_tail_prev);

    /* Call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    /* Postconditions */
    assert(popped != NULL);
    assert(popped == old_tail_prev);
    assert(aws_linked_list_is_valid(&list));

    if (old_list_has_one_node) {
        /* List becomes empty */
        assert(aws_linked_list_empty(&list));
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* List still has at least one node */
        assert(!aws_linked_list_empty(&list));
        /* The new tail.prev is the node before the popped node */
        assert(list.tail.prev == old_tail_prev->prev);
        /* The head remains unchanged */
        assert(list.head.next == old_head_next);
    }

    /* Sentinels remain valid */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
