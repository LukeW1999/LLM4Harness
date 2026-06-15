#include <assert.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    bool src_was_empty = (src.head.next == &src.tail);
    bool dst_was_empty = (dst.head.next == &dst.tail);

    aws_linked_list_move_all_back(&dst, &src);

    /* validate that both lists are still well‑formed */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* dst invariants */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    if (src_was_empty) {
        /* when src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* dst tail now points to the former last node of src */
        assert(dst.tail.prev == old_src.tail.prev);
        assert(dst.tail.prev->next == &dst.tail);

        if (dst_was_empty) {
            /* dst was empty, its head now points to src's first node */
            assert(dst.head.next == old_src.head.next);
            assert(old_src.head.next->prev == &dst.head);
        } else {
            /* both lists were non‑empty, they are concatenated */
            struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
            assert(old_dst_last->next == old_src.head.next);
            assert(old_src.head.next->prev == old_dst_last);
        }
    }

    /* a trivial reachable assertion to guarantee CBMC sees an assert */
    assert(1);
}
