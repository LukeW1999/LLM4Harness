#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness(void) {
    /* Allocate two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Initialize both lists */
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    /* Nondeterministically add nodes to list a */
    struct aws_linked_list_node node_a1;
    struct aws_linked_list_node node_a2;

    bool add_a1;
    bool add_a2;

    /* Use nondet booleans to decide whether to add nodes */
    if (add_a1) {
        aws_linked_list_push_back(&a, &node_a1);
    }
    if (add_a2) {
        aws_linked_list_push_back(&a, &node_a2);
    }

    /* Nondeterministically add nodes to list b */
    struct aws_linked_list_node node_b1;
    struct aws_linked_list_node node_b2;

    bool add_b1;
    bool add_b2;

    if (add_b1) {
        aws_linked_list_push_back(&b, &node_b1);
    }
    if (add_b2) {
        aws_linked_list_push_back(&b, &node_b2);
    }

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Record state before the call */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    /* Record first/last nodes of each list before swap */
    struct aws_linked_list_node *a_first_before = a.head.next;
    struct aws_linked_list_node *a_last_before  = a.tail.prev;
    struct aws_linked_list_node *b_first_before = b.head.next;
    struct aws_linked_list_node *b_last_before  = b.tail.prev;

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Postconditions (validity) */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Postconditions (emptiness is swapped) */
    assert(aws_linked_list_empty(&a) == b_was_empty);
    assert(aws_linked_list_empty(&b) == a_was_empty);

    /* Postconditions (content is swapped):
     * If b was non-empty, a's head.next should point to b's old first node
     * and a's tail.prev should point to b's old last node */
    if (!b_was_empty) {
        assert(a.head.next == b_first_before);
        assert(a.tail.prev == b_last_before);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);
    }

    /* If a was non-empty, b's head.next should point to a's old first node
     * and b's tail.prev should point to a's old last node */
    if (!a_was_empty) {
        assert(b.head.next == a_first_before);
        assert(b.tail.prev == a_last_before);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev->next == &b.tail);
    }

    /* Postconditions (frame): head/tail sentinel linkage is self-consistent */
    assert(a.head.next->prev == &a.head);
    assert(a.tail.prev->next == &a.tail);
    assert(b.head.next->prev == &b.head);
    assert(b.tail.prev->next == &b.tail);

    return 0;
}
