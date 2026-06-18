#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: both lists must be valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;
    bool src_was_empty = (old_src.head.next == &old_src.tail);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postcondition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Invariant fields that never change */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        /* When source was empty, destination must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        /* Source remains empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* After move, source must be empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* Destination head should stay the same as before */
        assert(dst.head.next == old_dst.head.next);

        /* The previous last node of the original destination now links to the first node of the original source */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);

        /* Destination tail now points to the original source's last node */
        assert(dst.tail.prev == old_src_last);
    }
}
