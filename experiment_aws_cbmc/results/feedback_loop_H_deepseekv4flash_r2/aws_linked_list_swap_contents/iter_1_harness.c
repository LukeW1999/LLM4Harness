#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <stdbool.h>
#include <stddef.h>

void aws_linked_list_swap_contents_harness() {
    /* Non-deterministically allocate and initialize two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Bound the size of the lists (number of nodes) */
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume validity (double-linked structure) for both lists */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save the original first and last node pointers for both lists */
    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last = b.tail.prev;

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Postcondition: both lists remain valid */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Postcondition: contents are swapped */
    /* Determine if the old lists were empty */
    bool old_a_empty = (old_a_first == &a.tail);
    bool old_b_empty = (old_b_first == &b.tail);

    if (old_b_empty) {
        /* If b was empty, after swap a must be empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* If b was nonempty, a must now contain exactly the nodes that were in b */
        assert(a.head.next == old_b_first);
        assert(a.tail.prev == old_b_last);
    }

    if (old_a_empty) {
        /* If a was empty, after swap b must be empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* If a was nonempty, b must now contain exactly the nodes that were in a */
        assert(b.head.next == old_a_first);
        assert(b.tail.prev == old_a_last);
    }

    /* No other fields to check (list has only head and tail) */
}
