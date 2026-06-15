#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound two linked lists */
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

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;
    bool src_was_empty = (old_src.head.next == &old_src.tail);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: validity invariants */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Post‑condition: behavior when src was empty */
    if (src_was_empty) {
        /* both lists unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(src.head.next == old_src.head.next);
        assert(src.tail.prev == old_src.tail.prev);
    } else {
        /* src becomes empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst now ends with the former src nodes */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    }

    /* 6. Unchanged fields for both lists */
    /* head.prev is always NULL */
    assert(dst.head.prev == NULL);
    assert(src.head.prev == NULL);
    /* tail.next is always NULL */
    assert(dst.tail.next == NULL);
    assert(src.tail.next == NULL);
}
