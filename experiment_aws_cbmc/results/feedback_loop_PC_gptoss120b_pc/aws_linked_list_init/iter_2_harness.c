#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound destination list */
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Allocate and bound source list */
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Snapshot old states (shallow copy) */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Invariants must hold after the call */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Source list must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == 0);
    assert(src.tail.next == 0);

    /* Determine whether source was originally empty */
    bool src_was_empty = (old_src.head.next == &old_src.tail);

    if (src_was_empty) {
        /* Destination list should be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* Destination list should now contain original dst elements followed by original src elements */

        /* Front of destination unchanged */
        assert(dst.head.next == old_dst.head.next);

        /* New tail of destination is the original tail of source */
        assert(dst.tail.prev == old_src.tail.prev);
        assert(dst.tail.prev->next == &dst.tail);
        assert(dst.tail.prev->prev == old_src.tail.prev->prev);

        /* Link between old dst tail and old src head */
        assert(old_dst.tail.prev->next == old_src.head.next);
        assert(old_src.head.next->prev == old_dst.tail.prev);
    }

    /* Head and tail sentinel invariants for destination */
    assert(dst.head.prev == 0);
    assert(dst.tail.next == 0);
}
