#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound the destination list */
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Allocate and bound the source list */
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Ensure the two lists are distinct */
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postconditions: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Determine whether source was empty before the call */
    if (old_src.head.next == &old_src.tail) {
        /* Source was empty: both lists should be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* Source was non‑empty: source becomes empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* Destination list gains the source nodes at its back */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);

        /* Head of destination list unchanged */
        assert(dst.head.next == old_dst.head.next);
    }
}
