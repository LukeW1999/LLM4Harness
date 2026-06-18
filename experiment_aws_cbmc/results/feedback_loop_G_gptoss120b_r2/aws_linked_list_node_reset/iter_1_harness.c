#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound the destination and source lists */
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

    struct aws_linked_list_node *old_dst_last = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last = src.tail.prev;

    bool src_was_empty = aws_linked_list_empty(&src);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postconditions: validity invariants */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Unchanged fields for both lists */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* If src was empty, dst should be unchanged */
    if (src_was_empty) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* dst.tail.prev should now be the former last node of src */
        assert(dst.tail.prev == old_src_last);
        /* The former last node of dst should now point to the former first node of src */
        assert(old_dst_last->next == old_src_first);
        /* The former first node of src should now point back to the former last node of dst */
        assert(old_src_first->prev == old_dst_last);
        /* The former last node of src should point to the dst tail sentinel */
        assert(old_src_last->next == &dst.tail);
    }
}
