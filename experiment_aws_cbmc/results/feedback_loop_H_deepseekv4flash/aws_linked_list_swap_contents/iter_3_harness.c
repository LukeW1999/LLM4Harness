#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness() {
    /* nondeterministic lists with small bound for tractability */
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* ensure both lists are allocated and bounded (small bound) */
    ensure_linked_list_is_allocated(&a, 2);
    ensure_linked_list_is_allocated(&b, 2);

    /* assume deep validity (bounded iteration) */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&a));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&b));

    /* save old emptiness and boundary pointers */
    bool a_was_empty = (a.head.next == &a.tail);
    bool b_was_empty = (b.head.next == &b.tail);
    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last = b.tail.prev;

    /* call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* postconditions */
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    /* emptiness is swapped */
    assert((a.head.next == &a.tail) == b_was_empty);
    assert((b.head.next == &b.tail) == a_was_empty);

    /* head and tail connections for a */
    if (b_was_empty) {
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        assert(a.head.next == old_b_first);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b_last);
        assert(a.tail.prev->next == &a.tail);
    }

    /* head and tail connections for b */
    if (a_was_empty) {
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        assert(b.head.next == old_a_first);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a_last);
        assert(b.tail.prev->next == &b.tail);
    }
}
