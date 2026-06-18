#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* The maximum number of nodes that can be allocated in a list for the harness.
 * This macro is normally defined by the build system; define a reasonable bound
 * if it is not already defined. */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the source and destination lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst); /* distinct objects */

    /* 2. Snapshot old state */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;   /* may be &old_dst.head if empty */
    struct aws_linked_list_node *old_src_front = old_src.head.next; /* may be &old_src.tail if empty */
    struct aws_linked_list_node *old_src_back  = old_src.tail.prev; /* may be &old_src.head if empty */

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* 5. Determine whether the source list was empty before the call */
    if (old_src.head.next == &old_src.tail) {
        /* Source was empty: destination must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);

        /* Source remains empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* Source was non‑empty: after the call source must be empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
        assert(src.head.prev == NULL);
        assert(src.tail.next == NULL);

        /* Destination head unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);

        /* The original last node of the old destination now links to the first node of the old source */
        assert(old_dst_last->next == old_src_front);
        assert(old_src_front->prev == old_dst_last);

        /* The original last node of the old source now links to the destination tail */
        assert(old_src_back->next == &dst.tail);
        assert(dst.tail.prev == old_src_back);
        assert(dst.tail.prev->next == &dst.tail);
    }
}
