#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness() {
    /* 1. Declare and initialize two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions: both lists must be valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state before calling */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save key pointers for postcondition checks */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* src must be empty after the call */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* dst must be valid */
    assert(aws_linked_list_is_valid(&dst));
    /* src must be valid */
    assert(aws_linked_list_is_valid(&src));

    if (!src_was_empty) {
        /* If src was non-empty, its nodes are now at the back of dst */
        /* The old dst back should now point to old src front */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* The old src back should now point to dst tail */
        assert(old_src_back->next == &dst.tail);
        assert(dst.tail.prev == old_src_back);

        if (dst_was_empty) {
            /* If dst was empty, dst head should now point to old src front */
            assert(dst.head.next == old_src_front);
            assert(old_src_front->prev == &dst.head);
        }
    } else {
        /* If src was empty, dst should be unchanged */
        assert(dst.tail.prev == old_dst_back);
    }
}
