#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_front = old_src.head.next;
    struct aws_linked_list_node *old_src_back = old_src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    if (old_src.head.next == &old_src.tail) {
        /* source was empty */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);

        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* source was non‑empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
        assert(src.head.prev == NULL);
        assert(src.tail.next == NULL);

        assert(dst.head.next == old_dst.head.next);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);

        assert(old_dst_last->next == old_src_front);
        assert(old_src_front->prev == old_dst_last);

        assert(old_src_back->next == &dst.tail);
        assert(dst.tail.prev == old_src_back);
        assert(dst.tail.prev->next == &dst.tail);
    }
}
