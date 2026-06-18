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

    /* 2. Save old state before the call */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save key pointers for postcondition checks */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;
    struct aws_linked_list_node *old_dst_front = dst.head.next;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* Postcondition: src is always valid and empty after the call */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    /* Postcondition: dst is always valid after the call */
    assert(aws_linked_list_is_valid(&dst));

    if (!src_was_empty) {
        /* src nodes were spliced into dst between old_dst_back and dst.tail */
        /* dst's back now points to old src_back */
        assert(dst.tail.prev == old_src_back);
        /* old src_back's next points to dst.tail */
        assert(old_src_back->next == &dst.tail);
        /* old dst_back's next points to old src_front */
        assert(old_dst_back->next == old_src_front);
        /* old src_front's prev points to old dst_back */
        assert(old_src_front->prev == old_dst_back);
        /* dst front is unchanged */
        assert(dst.head.next == old_dst_front);
    } else {
        /* src was empty: dst is unchanged */
        assert(dst.head.next == old_dst_front);
        assert(dst.tail.prev == old_dst_back);
    }

    /* src is reset: head.next points to tail and tail.prev points to head */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* src head and tail sentinel pointers are intact */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* dst head and tail sentinel pointers are intact */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
