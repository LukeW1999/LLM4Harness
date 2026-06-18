#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Declare and initialize dst and src linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    /* dst != src (AWS_RESTRICT) */
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save key pointers before the call */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* Validity invariants must hold after the call */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* src must be empty after the call */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (!src_was_empty) {
        /* If src was non-empty, its nodes were spliced into dst */
        /* dst's back now points to old src_back */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);

        /* old dst_back connects to old src_front */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);
    } else {
        /* If src was empty, dst is unchanged */
        assert(dst.tail.prev == old_dst_back);
    }

    /* If dst was empty and src was non-empty, dst's head.next should be old_src_front */
    if (dst_was_empty && !src_was_empty) {
        assert(dst.head.next == old_src_front);
        assert(old_src_front->prev == &dst.head);
    }
}
