#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the source and destination linked lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst); /* distinct objects */

    /* 2. Save old state */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *src_first = src.head.next;
    struct aws_linked_list_node *src_last  = src.tail.prev;
    struct aws_linked_list_node *dst_first = dst.head.next;
    struct aws_linked_list_node *dst_last  = dst.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* 5. Post‑condition: source list must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 6. Post‑condition: destination list must contain original dst nodes
       followed by original src nodes (if any) */
    if (src_first != &src.tail) {
        /* src was non‑empty, its nodes are now appended to dst */
        assert(dst.tail.prev == src_last);
        assert(src_last->next == &dst.tail);

        if (dst_first != &dst.tail) {
            /* dst was non‑empty */
            assert(src_first->prev == dst_last);
            assert(dst_last->next == src_first);
        } else {
            /* dst was empty */
            assert(src_first->prev == &dst.head);
            assert(dst.head.next == src_first);
        }
    } else {
        /* src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == old_dst.head.prev);
        assert(dst.tail.next == old_dst.tail.next);
    }

    /* 7. Unchanged fields for both lists (invariants) */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
