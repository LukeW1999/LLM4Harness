#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness(void) {
    /* Allocate and bound the first list */
    struct aws_linked_list a;
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&a));

    /* Allocate and bound the second list */
    struct aws_linked_list b;
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Assume the two list objects are distinct */
    __CPROVER_assume(&a != &b);

    /* Save old state */
    struct aws_linked_list a_old = a;
    struct aws_linked_list b_old = b;

    /* Record emptiness before the call */
    bool a_was_empty = (a_old.head.next == &a_old.tail);
    bool b_was_empty = (b_old.head.next == &b_old.tail);

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Post‑condition: unchanged fields of the list structs */
    assert(a.head.prev == a_old.head.prev);
    assert(a.tail.next == a_old.tail.next);
    assert(b.head.prev == b_old.head.prev);
    assert(b.tail.next == b_old.tail.next);

    /* Post‑condition: contents of the lists are swapped */
    if (b_was_empty) {
        /* a should become empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a should now contain b's original nodes */
        assert(a.head.next == b_old.head.next);
        assert(a.tail.prev == b_old.tail.prev);
        /* bidirectional links for a's new first and last nodes */
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);
    }

    if (a_was_empty) {
        /* b should become empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b should now contain a's original nodes */
        assert(b.head.next == a_old.head.next);
        assert(b.tail.prev == a_old.tail.prev);
        /* bidirectional links for b's new first and last nodes */
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev->next == &b.tail);
    }

    /* Validity invariants must hold after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));
}
