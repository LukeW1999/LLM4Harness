#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness() {
    /* 1. Allocate and bound the source and destination lists */
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

    struct aws_linked_list_node *src_head_next_old = src.head.next;
    struct aws_linked_list_node *src_tail_prev_old = src.tail.prev;
    struct aws_linked_list_node *dst_tail_prev_old = dst.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: validity invariants must hold */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* 5. Post‑condition: behavior depending on whether src was empty */
    if (src_head_next_old == &src.tail) {
        /* src was empty: both lists must be unchanged */
        assert(src.head.next == old_src.head.next);
        assert(src.tail.prev == old_src.tail.prev);
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src non‑empty: src must become empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
        assert(src.head.prev == NULL);
        assert(src.tail.next == NULL);

        /* dst must have the moved nodes appended */
        assert(dst.tail.prev == src_tail_prev_old);
        assert(dst_tail_prev_old->next == src_head_next_old);
        assert(src_head_next_old->prev == dst_tail_prev_old);
    }

    /* 6. Unchanged fields (structural invariants) */
    assert(&src.head == &old_src.head);
    assert(&src.tail == &old_src.tail);
    assert(&dst.head == &old_dst.head);
    assert(&dst.tail == &old_dst.tail);

    assert(src.head.prev == old_src.head.prev);
    assert(src.tail.next == old_src.tail.next);
    assert(dst.head.prev == old_dst.head.prev);
    assert(dst.tail.next == old_dst.tail.next);
}
