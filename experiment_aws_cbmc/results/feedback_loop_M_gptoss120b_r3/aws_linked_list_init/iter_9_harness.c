#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list src;
    struct aws_linked_list dst;

    aws_linked_list_init(&src);
    aws_linked_list_init(&dst);

    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *src_first = src.head.next;
    struct aws_linked_list_node *src_last  = src.tail.prev;
    struct aws_linked_list_node *dst_first = dst.head.next;
    struct aws_linked_list_node *dst_last  = dst.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_first != &src.tail) {
        assert(dst.tail.prev == src_last);
        assert(src_last->next == &dst.tail);

        if (dst_first != &dst.tail) {
            assert(src_first->prev == dst_last);
            assert(dst_last->next == src_first);
        } else {
            assert(src_first->prev == &dst.head);
            assert(dst.head.next == src_first);
        }
    } else {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == old_dst.head.prev);
        assert(dst.tail.next == old_dst.tail.next);
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
