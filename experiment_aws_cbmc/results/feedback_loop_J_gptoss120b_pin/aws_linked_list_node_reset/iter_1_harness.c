#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound the destination list */
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Allocate and bound the source list */
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity invariants must hold after the call */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Postconditions depending on whether the source list was empty */
    if (old_src.head.next == &old_src.tail) {
        /* Source was empty: both lists should be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* Source was non‑empty: source becomes empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* Destination tail now points to the former last node of source */
        assert(dst.tail.prev == old_src.tail.prev);
        /* The former last node of source points to the destination tail */
        assert(old_src.tail.prev->next == &dst.tail);

        /* The former last node of destination (could be head if empty) links to the first node of source */
        assert(old_dst.tail.prev->next == old_src.head.next);
        assert(old_src.head.next->prev == old_dst.tail.prev);

        /* Head of destination points to the correct first element */
        if (old_dst.head.next == &old_dst.tail) {
            /* Destination was empty before */
            assert(dst.head.next == old_src.head.next);
        } else {
            /* Destination had elements before */
            assert(dst.head.next == old_dst.head.next);
        }
    }
}
