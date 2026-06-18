#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Allocate and bound the lists */
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume the lists are valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));
    __CPROVER_assume(&a != &b);

    /* Save old state */
    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last  = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last  = b.tail.prev;

    bool old_a_empty = aws_linked_list_empty(&a);
    bool old_b_empty = aws_linked_list_empty(&b);

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 1. Validity invariants must hold */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* 2. a now contains the old contents of b */
    if (old_b_empty) {
        /* a must be empty */
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a should have b's former nodes */
        assert(a.head.next == old_b_first);
        assert(a.tail.prev == old_b_last);
        /* the first and last nodes are correctly linked to a's head/tail */
        assert(old_b_first->prev == &a.head);
        assert(old_b_last->next  == &a.tail);
    }

    /* 3. b now contains the old contents of a */
    if (old_a_empty) {
        /* b must be empty */
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b should have a's former nodes */
        assert(b.head.next == old_a_first);
        assert(b.tail.prev == old_a_last);
        /* the first and last nodes are correctly linked to b's head/tail */
        assert(old_a_first->prev == &b.head);
        assert(old_a_last->next  == &b.tail);
    }
}
