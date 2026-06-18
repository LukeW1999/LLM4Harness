#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, 5);
    ensure_linked_list_is_allocated(&src, 5);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (aws_linked_list_empty(&old_src)) {
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
    } else {
        assert(dst.tail.prev == old_src_last);
        if (aws_linked_list_empty(&old_dst)) {
            assert(dst.head.next == old_src_first);
            assert(old_src_first->prev == &dst.head);
        } else {
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
        }
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
