#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a, b;

    /* Allocate and initialize both lists with bounded size */
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save old state */
    bool old_a_empty = aws_linked_list_empty(&a);
    bool old_b_empty = aws_linked_list_empty(&b);
    struct aws_linked_list_node *old_a_head_next = a.head.next;
    struct aws_linked_list_node *old_a_tail_prev = a.tail.prev;
    struct aws_linked_list_node *old_b_head_next = b.head.next;
    struct aws_linked_list_node *old_b_tail_prev = b.tail.prev;

    /* Call function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Postcondition: a now contains b's old contents */
    if (old_b_empty) {
        assert(aws_linked_list_empty(&a));
    } else {
        assert(a.head.next == old_b_head_next);
        assert(a.tail.prev == old_b_tail_prev);
    }

    /* Postcondition: b now contains a's old contents */
    if (old_a_empty) {
        assert(aws_linked_list_empty(&b));
    } else {
        assert(b.head.next == old_a_head_next);
        assert(b.tail.prev == old_a_tail_prev);
    }

    /* Validity invariants hold after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
