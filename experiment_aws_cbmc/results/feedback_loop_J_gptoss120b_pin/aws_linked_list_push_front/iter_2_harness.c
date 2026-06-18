#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    /* Allocate and bound the destination and source linked lists */
    ensure_linked_list_is_allocated(&dst, 5);
    ensure_linked_list_is_allocated(&src, 5);

    /* Assume preconditions: both lists are valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state for later comparison */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;   /* may be &old_dst.head if empty */
    struct aws_linked_list_node *old_src_front = old_src.head.next; /* may be &old_src.tail if empty */
    struct aws_linked_list_node *old_src_back  = old_src.tail.prev;  /* may be &old_src.head if empty */

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postcondition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Postcondition: source list must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Determine whether source was originally empty */
    if (old_src.head.next == &old_src.tail) {
        /* Source was empty: destination must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* Source was non‑empty */

        /* Destination head unchanged */
        assert(dst.head.next == old_dst.head.next);

        /* Destination tail now points to the former source back node */
        assert(dst.tail.prev == old_src_back);

        /* The node that was previously the last of the original destination
           (old_dst_last) must now link to the first node of the original source */
        assert(old_dst_last->next == old_src_front);
        assert(old_src_front->prev == old_dst_last);

        /* The former source back node must link to the destination tail sentinel */
        assert(old_src_back->next == &dst.tail);
        assert(dst.tail.prev->next == &dst.tail);
    }
}
