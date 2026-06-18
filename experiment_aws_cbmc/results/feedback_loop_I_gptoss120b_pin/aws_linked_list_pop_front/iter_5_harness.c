#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    bool src_was_empty = (old_src_first == &old_src.tail);
    bool dst_was_empty = (old_dst_first == &old_dst.tail);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (src_was_empty) {
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
        assert(src.head.next == old_src_first);
        assert(src.tail.prev == old_src_last);
    } else {
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        assert(dst.tail.prev == old_src_last);
        assert(dst.tail.prev->next == &dst.tail);
        assert(old_src_last->next == &dst.tail);

        if (dst_was_empty) {
            assert(dst.head.next == old_src_first);
            assert(dst.head.next->prev == &dst.head);
        } else {
            assert(dst.head.next == old_dst_first);
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
        }
    }
}
