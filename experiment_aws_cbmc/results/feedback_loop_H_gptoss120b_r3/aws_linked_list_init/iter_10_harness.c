#include <aws/common/linked_list.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_head_next = dst.head.next;
    struct aws_linked_list_node *old_dst_tail_prev = dst.tail.prev;
    struct aws_linked_list_node *old_src_head_next = src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(aws_linked_list_empty(&src));

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (old_src_head_next == &old_src.tail) {
        assert(dst.head.next == old_dst_head_next);
        assert(dst.tail.prev == old_dst_tail_prev);
    } else {
        assert(dst.tail.prev == old_src_tail_prev);
        assert(dst.head.next == old_dst_head_next);
        if (old_dst_tail_prev) {
            assert(old_dst_tail_prev->next == old_src_head_next);
        }
        if (old_src_head_next) {
            assert(old_src_head_next->prev == old_dst_tail_prev);
        }
    }
}
