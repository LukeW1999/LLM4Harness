#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocator.h>

void aws_linked_list_move_all_back_harness() {
    /* Non-deterministic lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    /* Allocate and initialize lists with bounded nondeterministic structure */
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions: both lists are valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save state before call */
    bool src_empty_before = aws_linked_list_empty(&src);
    bool dst_empty_before = aws_linked_list_empty(&dst);
    struct aws_linked_list_node *old_dst_head_next = dst.head.next;
    struct aws_linked_list_node *old_dst_tail_prev = dst.tail.prev;
    struct aws_linked_list_node *old_src_head_next = src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = src.tail.prev;

    /* Call function under proof */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postconditions */

    /* 1. Validity invariants */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 2. src becomes empty */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 3. dst front unchanged */
    assert(dst.head.next == old_dst_head_next);

    /* 4. dst back changes appropriately */
    if (!src_empty_before) {
        /* If src was non-empty, the new last element of dst is the old last element of src */
        assert(dst.tail.prev == old_src_tail_prev);
        /* The old first element of src now points backward to the old last element of dst (or dst head if dst was empty) */
        if (dst_empty_before) {
            assert(old_src_head_next->prev == &dst.head);
        } else {
            assert(old_src_head_next->prev == old_dst_tail_prev);
        }
        /* The old last element of src points forward to dst tail */
        assert(old_src_tail_prev->next == &dst.tail);
    } else {
        /* If src was empty, dst tail unchanged */
        assert(dst.tail.prev == old_dst_tail_prev);
    }

    /* 5. If dst was empty and src non-empty, dst becomes non-empty */
    if (dst_empty_before && !src_empty_before) {
        assert(!aws_linked_list_empty(&dst));
    }
}
