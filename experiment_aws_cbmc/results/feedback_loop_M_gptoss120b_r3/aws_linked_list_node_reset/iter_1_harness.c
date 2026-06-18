#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list src;
    struct aws_linked_list dst;

    /* Allocate linked lists with bounded length */
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are initially valid */
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Ensure the two list objects are distinct */
    __CPROVER_assume(&src != &dst);

    /* Save old state for post‑condition checks */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *src_first = src.head.next;
    struct aws_linked_list_node *src_last  = src.tail.prev;
    struct aws_linked_list_node *dst_first = dst.head.next;
    struct aws_linked_list_node *dst_last  = dst.tail.prev;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* -------------------------------------------------------------------- */
    /* Post‑conditions */

    /* 1. Source list must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* 2. Destination list must remain a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&dst));

    /* 3. Behaviour depending on whether the source list was empty */
    if (src_first == &src.tail) {
        /* Source was empty: destination unchanged */
        assert(dst.head.next == dst_first);
        assert(dst.tail.prev == dst_last);
    } else {
        /* Source was non‑empty: its nodes are appended to destination */

        /* Destination tail now points to the former last node of source */
        assert(dst.tail.prev == src_last);

        /* Destination head unchanged */
        assert(dst.head.next == dst_first);

        /* Linkage between old destination last node and first source node */
        if (dst_last == &dst.head) {
            /* Destination was empty before the move */
            assert(dst.head.next == src_first);
        } else {
            assert(dst_last->next == src_first);
        }

        /* The first node from source now points back to the old destination last */
        if (dst_last == &dst.head) {
            assert(src_first->prev == &dst.head);
        } else {
            assert(src_first->prev == dst_last);
        }

        /* The former last node of source now points to the destination tail */
        assert(src_last->next == &dst.tail);
    }

    /* 4. Both lists must satisfy the validity invariant after the operation */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));
}
