#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound source and destination linked lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    /* Save old copies for post‑condition checks */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity must hold for both lists after the call */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* Source list must be empty after the move */
    assert(aws_linked_list_empty(&src));

    /* Unchanged structural fields (head/tail sentinel invariants) */
    assert(src.head.prev == old_src.head.prev);
    assert(src.tail.next == old_src.tail.next);
    assert(dst.head.prev == old_dst.head.prev);
    assert(dst.tail.next == old_dst.tail.next);

    /* Post‑conditions depending on whether source was initially empty */
    if (aws_linked_list_empty(&old_src)) {
        /* When source is empty, destination must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* Destination now ends with the former last node of source */
        assert(dst.tail.prev == old_src.tail.prev);

        /* The node that previously was the last of destination (if any) now links to the former first node of source */
        struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
        struct aws_linked_list_node *old_src_first = old_src.head.next;

        if (!aws_linked_list_empty(&old_dst)) {
            assert(old_dst_last->next == old_src_first);
        } else {
            /* Destination was empty, so its first node is now the former first node of source */
            assert(dst.head.next == old_src_first);
        }
    }
}
