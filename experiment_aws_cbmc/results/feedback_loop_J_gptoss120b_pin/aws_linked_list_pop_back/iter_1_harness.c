#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Determine whether src was empty before the call */
    bool src_was_empty = (old_src.head.next == &old_src.tail);

    if (src_was_empty) {
        /* When src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* When src was non‑empty, dst's front stays the same */
        assert(dst.head.next == old_dst.head.next);

        /* dst's new back is the former back of src */
        assert(dst.tail.prev == old_src.tail.prev);

        /* The previous last node of dst (if any) must now point to src's first node */
        if (old_dst.tail.prev != &old_dst.head) {
            assert(old_dst.tail.prev->next == old_src.head.next);
        }

        /* src's first node must now point back to the previous last node of dst */
        assert(old_src.head.next->prev == old_dst.tail.prev);
    }

    /* Invariant fields that never change */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
