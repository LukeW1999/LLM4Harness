#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last = old_src.tail.prev;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity invariants */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* dst head should remain unchanged */
    assert(dst.head.next == old_dst.head.next);
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    if (old_src_first == &old_src.tail) {
        /* src was originally empty: dst unchanged */
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(old_dst_last->next == &dst.tail);
    } else {
        /* src was non‑empty: dst now ends with src's former last node */
        assert(dst.tail.prev == old_src_last);
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
        assert(old_src_last->next == &dst.tail);
    }
}
