#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Allocate and bound the lists */
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are deeply valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&a));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&b));

    /* The function requires that a and b do not alias */
    __CPROVER_assume(&a != &b);

    /* Save old states for postcondition checks */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* Save the first and last nodes of each list (may be sentinels if empty) */
    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last  = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last  = b.tail.prev;

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 1. Both lists must remain deeply valid */
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    /* 2. The sentinel nodes' prev (head) and next (tail) are never modified */
    assert(a.head.prev == old_a.head.prev);
    assert(a.tail.next == old_a.tail.next);
    assert(b.head.prev == old_b.head.prev);
    assert(b.tail.next == old_b.tail.next);

    /* 3. The contents of a are now the old contents of b */
    if (old_b_first == &old_b.tail) {
        /* old b was empty → a must be empty */
        assert(aws_linked_list_empty(&a));
    } else {
        /* old b was non‑empty → a now holds those nodes */
        assert(a.head.next == old_b_first);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b_last);
        assert(a.tail.prev->next == &a.tail);
    }

    /* 4. The contents of b are now the old contents of a */
    if (old_a_first == &old_a.tail) {
        /* old a was empty → b must be empty */
        assert(aws_linked_list_empty(&b));
    } else {
        /* old a was non‑empty → b now holds those nodes */
        assert(b.head.next == old_a_first);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a_last);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 5. The old first/last nodes of a are now correctly linked into b (already covered above) */
    /* 6. The old first/last nodes of b are now correctly linked into a (already covered above) */
}
