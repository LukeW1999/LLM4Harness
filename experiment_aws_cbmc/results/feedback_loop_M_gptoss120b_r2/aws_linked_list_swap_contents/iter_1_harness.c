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

    /* Validity must hold for both lists after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Post‑conditions for list a (receives b's former contents) */
    if (old_b_first == &old_b.tail) {
        /* b was empty → a must become empty */
        assert(aws_linked_list_empty(&a));
    } else {
        /* a now contains the nodes that were in b */
        assert(a.head.next == old_b_first);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b_last);
        assert(a.tail.prev->next == &a.tail);
    }

    /* Post‑conditions for list b (receives a's former contents) */
    if (old_a_first == &old_a.tail) {
        /* a was empty → b must become empty */
        assert(aws_linked_list_empty(&b));
    } else {
        /* b now contains the nodes that were in a */
        assert(b.head.next == old_a_first);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a_last);
        assert(b.tail.prev->next == &b.tail);
    }

    /* Unchanged fields: the sentinel nodes themselves remain the same objects */
    assert(&a.head == &old_a.head);
    assert(&a.tail == &old_a.tail);
    assert(&b.head == &old_b.head);
    assert(&b.tail == &old_b.tail);
}
