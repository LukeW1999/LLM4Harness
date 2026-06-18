#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    bool old_dst_empty = aws_linked_list_empty(&old_dst);
    bool old_src_empty = aws_linked_list_empty(&old_src);

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last = old_src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (old_src_empty) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        assert(aws_linked_list_empty(&src));
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);

        if (old_dst_empty) {
            assert(dst.head.next == old_src_first);
            assert(old_src_first->prev == &dst.head);
        } else {
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
        }
    }
}
