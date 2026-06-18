#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are valid before the call and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Snapshot old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    bool src_was_empty = aws_linked_list_empty(&src);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the operation */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (src_was_empty) {
        /* When src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
        /* Nodes of dst must still be linked correctly */
        if (old_dst_first != &dst.tail) {
            assert(old_dst_first->prev == &dst.head);
        }
        if (old_dst_last != &dst.head) {
            assert(old_dst_last->next == &dst.tail);
        }
    } else {
        /* When src was non‑empty, dst must contain original dst followed by src */
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_src_last);

        /* Link between old dst last and old src first must be established */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    }

    /* Unchanged fields of dst that are not part of the splice */
    /* head.prev and tail.next are invariant by definition */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
