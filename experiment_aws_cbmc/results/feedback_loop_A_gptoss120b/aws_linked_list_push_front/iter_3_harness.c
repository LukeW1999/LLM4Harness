#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    size_t dst_len = nondet_uint();
    size_t src_len = nondet_uint();

    __CPROVER_assume(dst_len <= MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(src_len <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    ensure_linked_list_is_allocated(&dst, dst_len);
    ensure_linked_list_is_allocated(&src, src_len);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Save old state */
    struct aws_linked_list_node *old_dst_head_next = dst.head.next;
    struct aws_linked_list_node *old_dst_tail_prev = (dst_len > 0) ? dst.tail.prev : NULL;
    struct aws_linked_list_node *old_src_head_next = src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = (src_len > 0) ? src.tail.prev : NULL;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑conditions: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(aws_linked_list_empty(&src));

    /* dst reflects the splice */
    if (src_len == 0) {
        /* src was empty → dst unchanged */
        assert(dst.head.next == old_dst_head_next);
        assert(dst.tail.prev == old_dst_tail_prev);
    } else {
        if (dst_len == 0) {
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

    /* Unchanged invariant fields for both lists */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Consistency of the new first and last nodes (guard against empty dst) */
    if (!aws_linked_list_empty(&dst)) {
        assert(dst.head.next->prev == &dst.head);
        assert(dst.tail.prev->next == &dst.tail);
    }
}
