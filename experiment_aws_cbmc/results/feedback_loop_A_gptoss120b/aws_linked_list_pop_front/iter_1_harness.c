#include <assert.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    bool src_was_empty = (src.head.next == &src.tail);
    bool dst_was_empty = (dst.head.next == &dst.tail);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Invariant fields that never change */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        /* When src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* dst.tail.prev must now be the former last node of src */
        assert(dst.tail.prev == old_src.tail.prev);
        assert(dst.tail.prev->next == &dst.tail);

        if (dst_was_empty) {
            /* dst was empty, so its first element is the former first element of src */
            assert(dst.head.next == old_src.head.next);
            assert(old_src.head.next->prev == &dst.head);
        } else {
            /* The former last node of dst now links to the former first node of src */
            struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
            assert(old_dst_last->next == old_src.head.next);
            assert(old_src.head.next->prev == old_dst_last);
        }
    }
}
