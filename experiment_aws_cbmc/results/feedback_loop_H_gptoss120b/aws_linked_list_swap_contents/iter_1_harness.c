#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_swap_contents_harness(void) {
    /* 1. Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    struct aws_linked_list_node *old_a_head_next = a.head.next;
    struct aws_linked_list_node *old_a_tail_prev = a.tail.prev;
    struct aws_linked_list_node *old_b_head_next = b.head.next;
    struct aws_linked_list_node *old_b_tail_prev = b.tail.prev;

    /* 3. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Post‑condition: validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* 5. Post‑condition: contents of the lists are swapped */
    if (aws_linked_list_empty(&old_b)) {
        /* old B was empty → A must be empty after the swap */
        assert(aws_linked_list_empty(&a));
    } else {
        /* A should now contain the nodes that were in old B */
        assert(a.head.next == old_b_head_next);
        assert(a.tail.prev == old_b_tail_prev);
        /* bidirectional links for the new first and last nodes */
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);
    }

    if (aws_linked_list_empty(&old_a)) {
        /* old A was empty → B must be empty after the swap */
        assert(aws_linked_list_empty(&b));
    } else {
        /* B should now contain the nodes that were in old A */
        assert(b.head.next == old_a_head_next);
        assert(b.tail.prev == old_a_tail_prev);
        /* bidirectional links for the new first and last nodes */
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 6. Unchanged fields: the list structures themselves (head/tail objects) stay at the same address */
    assert(&a.head == &old_a.head);
    assert(&a.tail == &old_a.tail);
    assert(&b.head == &old_b.head);
    assert(&b.tail == &old_b.tail);
}
