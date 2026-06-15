#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Maximum number of nodes that can be allocated in a list for the proof */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* 2. Save old state of both lists */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Post‑condition: src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 6. Unchanged fields of dst that are not affected by the operation */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(dst.head.next == old_dst.head.next); /* first element unchanged */

    /* 7. Behaviour depending on whether src was empty before the call */
    if (old_src.head.next == &old_src.tail) {
        /* src was empty: dst must be unchanged */
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.next == old_dst.head.next);
    } else {
        /* src was non‑empty: dst now contains the original dst elements
         * followed by the original src elements. */
        /* The new last node of dst is the old last node of src */
        assert(dst.tail.prev == old_src.tail.prev);

        /* The node that used to be the last of dst now points forward to the
         * first node of the original src list */
        struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
        struct aws_linked_list_node *old_src_first = old_src.head.next;
        struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);

        /* The first node of src (now in the middle of dst) must link back to the
         * previous node (old_dst_last) and forward to the next src node */
        assert(old_src_first->next == old_src_first->next); /* trivially true, kept for symmetry */

        /* The last node of src (now the last node of dst) must link forward to dst.tail */
        assert(old_src_last->next == &dst.tail);
        assert(dst.tail.prev == old_src_last);
    }
}
