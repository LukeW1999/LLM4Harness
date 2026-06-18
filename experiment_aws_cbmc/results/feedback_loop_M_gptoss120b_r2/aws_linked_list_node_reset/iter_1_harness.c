#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Bounding constant is provided by the build system */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src); /* distinct objects */

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Invariant fields that never change */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 6. After the move, source list must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* 7. Behaviour when source was empty before the call */
    if (old_src.head.next == &old_src.tail) {
        /* source empty → destination unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* 8. Source was non‑empty: destination now ends with the former source tail */
        assert(dst.tail.prev == old_src.tail.prev);

        /* 9. Linkage between the old destination tail and the old source head */
        if (old_dst.head.next != &old_dst.tail) {
            /* destination was non‑empty before */
            struct aws_linked_list_node *old_last_dst = old_dst.tail.prev;
            assert(old_last_dst->next == old_src.head.next);
            assert(old_src.head.next->prev == old_last_dst);
        } else {
            /* destination was empty before */
            assert(dst.head.next == old_src.head.next);
        }
    }
}
