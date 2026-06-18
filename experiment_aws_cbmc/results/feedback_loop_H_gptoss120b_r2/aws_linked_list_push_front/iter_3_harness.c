#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    if (!aws_linked_list_empty(&old_src)) {
        assert(dst.tail.prev == old_src.tail.prev);
        assert(old_src.tail.prev->next == &dst.tail);

        if (!aws_linked_list_empty(&old_dst)) {
            assert(old_dst.tail.prev->next == old_src.head.next);
            assert(old_src.head.next->prev == old_dst.tail.prev);
        } else {
            assert(dst.head.next == old_src.head.next);
            assert(old_src.head.next->prev == &dst.head);
        }
    } else {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        if (!aws_linked_list_empty(&dst)) {
            assert(dst.head.next->prev == &dst.head);
            assert(dst.tail.prev->next == &dst.tail);
        }
    }
}
