#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound the source and destination linked lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: both lists must be valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    /* Save old states for later comparison */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    /* Capture old boundary nodes for easier reasoning */
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;
    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postcondition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* Invariant: head.prev and tail.next are never modified */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    /* Determine whether the source list was originally empty */
    if (old_src.head.next == &old_src.tail) {
        /* Source was empty: both lists should be unchanged */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
        if (old_dst_first != &old_dst.tail) {
            /* non‑empty destination: internal links unchanged */
            assert(old_dst_first->prev == &dst.head);
            assert(old_dst_last->next == &dst.tail);
        } else {
            /* empty destination remains empty */
            assert(dst.head.next == &dst.tail);
            assert(dst.tail.prev == &dst.head);
        }
    } else {
        /* Source was non‑empty: source must become empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* Destination must now contain original dst nodes followed by original src nodes */
        if (old_dst_first == &old_dst.tail) {
            /* Destination was originally empty */
            assert(dst.head.next == old_src_first);
            assert(dst.tail.prev == old_src_last);
            assert(old_src_first->prev == &dst.head);
            assert(old_src_last->next == &dst.tail);
        } else {
            /* Destination originally non‑empty */
            assert(dst.head.next == old_dst_first);
            assert(dst.tail.prev == old_src_last);
            assert(old_dst_first->prev == &dst.head);
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
            assert(old_src_last->next == &dst.tail);
        }
    }
}
