#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_src_front = NULL;
    struct aws_linked_list_node *old_src_back = NULL;
    if (!aws_linked_list_empty(&src)) {
        old_src_front = src.head.next;
        old_src_back = src.tail.prev;
    }

    struct aws_linked_list_node *old_dst_back = NULL;
    if (!aws_linked_list_empty(&dst)) {
        old_dst_back = dst.tail.prev;
    }

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(aws_linked_list_is_valid(&src));

    assert(aws_linked_list_is_valid(&dst));

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (old_src_front == NULL) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);
        if (old_dst_back != NULL) {
            assert(old_dst_back->next == old_src_front);
            assert(old_src_front->prev == old_dst_back);
        } else {
            assert(dst.head.next == old_src_front);
            assert(old_src_front->prev == &dst.head);
        }
    }
}
