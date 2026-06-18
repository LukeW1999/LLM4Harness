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
    /* dst != src (AWS_RESTRICT precondition) */
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    bool src_was_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* src must always be empty after the call */
    assert(aws_linked_list_empty(&src));

    /* src must be valid */
    assert(aws_linked_list_is_valid(&src));

    /* dst must be valid */
    assert(aws_linked_list_is_valid(&dst));

    if (src_was_empty) {
        /* If src was empty, dst should be unchanged in terms of structure */
        /* dst's back should still be old_dst_back */
        assert(dst.tail.prev == old_dst_back);
    } else {
        /* If src was not empty, src nodes were spliced into dst */
        /* The back of dst should now be old_src_back */
        assert(dst.tail.prev == old_src_back);
        /* old_src_back's next should point to dst's tail */
        assert(old_src_back->next == &dst.tail);
        /* old_dst_back's next should point to old_src_front */
        assert(old_dst_back->next == old_src_front);
        /* old_src_front's prev should point to old_dst_back */
        assert(old_src_front->prev == old_dst_back);
    }

    /* src head and tail should be properly reset */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
