#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    /* 1. Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* The two list objects must be distinct */
    __CPROVER_assume(&a != &b);

    /* 2. Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    struct aws_linked_list_node *old_a_first = old_a.head.next;
    struct aws_linked_list_node *old_a_last  = old_a.tail.prev;

    struct aws_linked_list_node *old_b_first = old_b.head.next;
    struct aws_linked_list_node *old_b_last  = old_b.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Post‑condition checks */

    /* Both lists must remain valid */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* If the original B was empty, A must become empty */
    if (aws_linked_list_empty(&old_b)) {
        assert(aws_linked_list_empty(&a));
    } else {
        /* A now contains the former contents of B */
        assert(a.head.next == old_b_first);
        assert(a.tail.prev == old_b_last);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);
    }

    /* If the original A was empty, B must become empty */
    if (old_a_first == &old_a.tail) {
        assert(aws_linked_list_empty(&b));
    } else {
        /* B now contains the former contents of A */
        assert(b.head.next == old_a_first);
        assert(b.tail.prev == old_a_last);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev->next == &b.tail);
    }
}
