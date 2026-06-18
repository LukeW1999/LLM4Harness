#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, 5, allocator);
    ensure_linked_list_is_allocated(&dst, 5, allocator);

    /* Save old state */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *old_src_head_next = src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = src.tail.prev;
    struct aws_linked_list_node *old_dst_head_next = dst.head.next;
    struct aws_linked_list_node *old_dst_tail_prev = dst.tail.prev;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* dst head and tail sentinel invariants unchanged */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    /* dst head.next (first element) is unchanged */
    assert(dst.head.next == old_dst_head_next);

    /* Determine whether src was originally empty */
    if (old_src_head_next == &src.tail) {
        /* src was empty: dst unchanged */
        assert(dst.tail.prev == old_dst_tail_prev);
    } else {
        /* src was non‑empty: dst.tail.prev now points to the original last node of src */
        assert(dst.tail.prev == old_src_tail_prev);
        /* The node that was the original last of dst now points to the first node of src */
        assert(old_dst_tail_prev->next == old_src_head_next);
        /* The first node of src now points back to the original last of dst */
        assert(old_src_head_next->prev == old_dst_tail_prev);
        /* The original last node of src now points to dst.tail */
        assert(old_src_tail_prev->next == &dst.tail);
    }
}
