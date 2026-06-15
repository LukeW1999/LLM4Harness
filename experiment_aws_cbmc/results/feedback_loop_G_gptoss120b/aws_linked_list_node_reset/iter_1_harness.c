#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness() {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = dst.head.next;
    struct aws_linked_list_node *old_dst_last  = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: validity invariants */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Post‑condition: src must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* 6. Post‑condition: dst reflects the splice */
    if (old_src.head.next == &old_src.tail) {
        /* src was empty → dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src non‑empty → its nodes are appended to dst */
        /* New tail points to the former last node of src */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);

        if (old_dst_last != &dst.head) {
            /* dst was non‑empty before splice */
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
        } else {
            /* dst was empty before splice */
            assert(dst.head.next == old_src_first);
            assert(old_src_first->prev == &dst.head);
        }

        /* The original first element of dst (if any) stays at the front */
        if (old_dst_first != &dst.tail) {
            assert(dst.head.next == old_dst_first);
        }
    }

    /* 7. Unchanged fields that are not mentioned as modified */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
