#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* RESTRICT requires non-aliasing */
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* (a) Validity invariants always hold */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* (b) src is emptied */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* (c) src nodes moved to back of dst (if src was not empty) */
    if (!aws_linked_list_empty(&old_src)) {
        struct aws_linked_list_node *old_dst_back = old_dst.tail.prev;
        struct aws_linked_list_node *old_src_front = old_src.head.next;
        struct aws_linked_list_node *old_src_back = old_src.tail.prev;

        /* dst back now links to src front */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* src back now linked to dst tail */
        assert(old_src_back->next == &dst.tail);
        assert(dst.tail.prev == old_src_back);

        /* dst head.next unchanged */
        assert(dst.head.next == old_dst.head.next);
    } else {
        /* src was empty, dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.head.prev == old_dst.head.prev);
        assert(dst.tail.next == old_dst.tail.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    }
}
