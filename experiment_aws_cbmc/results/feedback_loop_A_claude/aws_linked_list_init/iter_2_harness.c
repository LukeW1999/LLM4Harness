#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Declare and initialize two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save pointers for checking linkage after the call */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* src must always be empty after the call */
    assert(aws_linked_list_empty(&src));

    /* src head and tail must point to each other */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (!src_was_empty) {
        /* src nodes were moved to dst back */
        /* old_dst_back should now link to old_src_front */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* old_src_back should now link to dst tail */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);

        /* dst should not be empty */
        assert(!aws_linked_list_empty(&dst));
    } else {
        /* src was empty, dst should be unchanged */
        if (dst_was_empty) {
            assert(aws_linked_list_empty(&dst));
        } else {
            assert(!aws_linked_list_empty(&dst));
        }
        /* dst back pointer unchanged */
        assert(dst.tail.prev == old_dst_back);
    }

    /* head.prev is always NULL for a valid list */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
