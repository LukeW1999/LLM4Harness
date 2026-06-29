#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness(void) {
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Initialize both lists */
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    /* Optionally add nodes to list a */
    struct aws_linked_list_node a_node1;
    struct aws_linked_list_node a_node2;

    /* Nondeterministically decide how many nodes to add to a */
    bool add_a1;
    bool add_a2;
    __CPROVER_assume(add_a1 == add_a1); /* unconstrained */

    if (add_a1) {
        aws_linked_list_push_back(&a, &a_node1);
    }
    if (add_a2) {
        aws_linked_list_push_back(&a, &a_node2);
    }

    /* Optionally add nodes to list b */
    struct aws_linked_list_node b_node1;
    struct aws_linked_list_node b_node2;

    bool add_b1;
    bool add_b2;
    __CPROVER_assume(add_b1 == add_b1); /* unconstrained */

    if (add_b1) {
        aws_linked_list_push_back(&b, &b_node1);
    }
    if (add_b2) {
        aws_linked_list_push_back(&b, &b_node2);
    }

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save state before swap */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    /* Save the first and last nodes of each list before swap */
    struct aws_linked_list_node *a_old_first = a.head.next;
    struct aws_linked_list_node *a_old_last = a.tail.prev;
    struct aws_linked_list_node *b_old_first = b.head.next;
    struct aws_linked_list_node *b_old_last = b.tail.prev;

    /* Perform the swap */
    aws_linked_list_swap_contents(&a, &b);

    /* Postcondition 1: Both lists are still valid after swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Postcondition 2: If a was empty before, b should be empty after */
    assert(a_was_empty == aws_linked_list_empty(&b));

    /* Postcondition 3: If b was empty before, a should be empty after */
    assert(b_was_empty == aws_linked_list_empty(&a));

    /* Postcondition 4: Head and tail sentinels are properly linked */
    /* a's head.next->prev should point back to a's head */
    assert(a.head.next->prev == &a.head);
    /* a's tail.prev->next should point back to a's tail */
    assert(a.tail.prev->next == &a.tail);
    /* b's head.next->prev should point back to b's head */
    assert(b.head.next->prev == &b.head);
    /* b's tail.prev->next should point back to b's tail */
    assert(b.tail.prev->next == &b.tail);

    /* Postcondition 5: Contents were actually swapped */
    /* If b was not empty before swap, a should now start with b's old first node */
    if (!b_was_empty) {
        assert(a.head.next == b_old_first);
        assert(a.tail.prev == b_old_last);
    }

    /* If a was not empty before swap, b should now start with a's old first node */
    if (!a_was_empty) {
        assert(b.head.next == a_old_first);
        assert(b.tail.prev == a_old_last);
    }

    /* Postcondition 6: If b was empty before, a should now be empty (head.next == &a.tail) */
    if (b_was_empty) {
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    }

    /* Postcondition 7: If a was empty before, b should now be empty (head.next == &b.tail) */
    if (a_was_empty) {
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    }

    /* Postcondition 8: head.next is not NULL for both lists */
    assert(a.head.next != NULL);
    assert(a.tail.prev != NULL);
    assert(b.head.next != NULL);
    assert(b.tail.prev != NULL);

    /* Postcondition 9: head.prev and tail.next remain NULL (sentinel invariant) */
    assert(a.head.prev == NULL);
    assert(a.tail.next == NULL);
    assert(b.head.prev == NULL);
    assert(b.tail.next == NULL);
}
