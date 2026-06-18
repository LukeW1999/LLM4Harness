#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness() {
    /* Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    struct aws_linked_list_node *old_a_first = old_a.head.next;
    struct aws_linked_list_node *old_a_last  = old_a.tail.prev;
    struct aws_linked_list_node *old_b_first = old_b.head.next;
    struct aws_linked_list_node *old_b_last  = old_b.tail.prev;

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Validity must hold for both lists after the call */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Unchanged fields: head.prev and tail.next for both lists */
    assert(a.head.prev == old_a.head.prev);
    assert(a.tail.next == old_a.tail.next);
    assert(b.head.prev == old_b.head.prev);
    assert(b.tail.next == old_b.tail.next);

    /* Post‑conditions for list A */
    if (aws_linked_list_empty(&old_b)) {
        /* B was empty, A must be empty now */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* A now contains B's former contents */
        assert(a.head.next == old_b_first);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b_last);
        assert(a.tail.prev->next == &a.tail);
    }

    /* Post‑conditions for list B */
    if (aws_linked_list_empty(&old_a)) {
        /* A was empty, B must be empty now */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* B now contains A's former contents */
        assert(b.head.next == old_a_first);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a_last);
        assert(b.tail.prev->next == &b.tail);
    }
}
