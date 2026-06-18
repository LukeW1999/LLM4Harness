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
    /* dst and src must be different (AWS_RESTRICT) */
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save key pointers before the call */
    struct aws_linked_list_node *dst_old_back = dst.tail.prev;
    struct aws_linked_list_node *src_old_front = src.head.next;
    struct aws_linked_list_node *src_old_back = src.tail.prev;

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
        /* dst's new back should be src's old back */
        assert(dst.tail.prev == src_old_back);
        /* src's old back should point to dst's tail */
        assert(src_old_back->next == &dst.tail);
        /* dst's old back should point to src's old front */
        assert(dst_old_back->next == src_old_front);
        /* src's old front should point back to dst's old back */
        assert(src_old_front->prev == dst_old_back);
        /* dst is non-empty */
        assert(!aws_linked_list_empty(&dst));
    } else {
        /* If src was empty, dst is unchanged */
        assert(dst.tail.prev == dst_old_back);
        /* dst emptiness is preserved */
        assert(aws_linked_list_empty(&dst) == dst_was_empty);
    }
}
