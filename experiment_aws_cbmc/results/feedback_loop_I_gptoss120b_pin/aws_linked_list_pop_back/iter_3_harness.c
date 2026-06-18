#define AWS_STATIC_IMPL
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;
    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    if (old_src.head.next == &old_src.tail) {
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
        if (old_dst_first != &old_dst.tail) {
            assert(old_dst_first->prev == &dst.head);
            assert(old_dst_last->next == &dst.tail);
        } else {
            assert(dst.head.next == &dst.tail);
            assert(dst.tail.prev == &dst.head);
        }
    } else {
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        if (old_dst_first == &old_dst.tail) {
            assert(dst.head.next == old_src_first);
            assert(dst.tail.prev == old_src_last);
            assert(old_src_first->prev == &dst.head);
            assert(old_src_last->next == &dst.tail);
        } else {
            assert(dst.head.next == old_dst_first);
            assert(dst.tail.prev == old_src_last);
            assert(old_dst_first->prev == &dst.head);
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
            assert(old_src_last->next == &dst.tail);
        }
    }
}
