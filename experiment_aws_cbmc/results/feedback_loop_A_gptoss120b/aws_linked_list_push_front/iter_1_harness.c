#include <assert.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_head_next = dst.head.next;
    struct aws_linked_list_node *old_dst_tail_prev = dst.tail.prev;
    struct aws_linked_list_node *old_src_head_next = src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = src.tail.prev;

    bool old_dst_empty = aws_linked_list_empty(&old_dst);
    bool old_src_empty = aws_linked_list_empty(&old_src);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Post‑condition: src is empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 6. Post‑condition: dst reflects the splice */
    if (old_src_empty) {
        /* src was empty → dst unchanged */
        assert(dst.head.next == old_dst_head_next);
        assert(dst.tail.prev == old_dst_tail_prev);
    } else {
        /* src non‑empty */
        if (old_dst_empty) {
            /* dst was empty → its first element is the former src head */
            assert(dst.head.next == old_src_head_next);
        } else {
            /* dst non‑empty → its first element unchanged */
            assert(dst.head.next == old_dst_head_next);
            /* link between old dst tail and old src head */
            assert(old_dst_tail_prev->next == old_src_head_next);
            assert(old_src_head_next->prev == old_dst_tail_prev);
        }
        /* new last element of dst is the former src tail */
        assert(dst.tail.prev == old_src_tail_prev);
        /* src tail now points to dst tail */
        assert(old_src_tail_prev->next == &dst.tail);
    }

    /* 7. Unchanged invariant fields for both lists */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 8. Consistency of the new first and last nodes */
    assert(dst.head.next->prev == &dst.head);
    assert(dst.tail.prev->next == &dst.tail);
}
