#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

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

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst is valid after move");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src is valid after move");
    __CPROVER_assert(aws_linked_list_empty(&src), "src is empty after move");

    if (!aws_linked_list_empty(&old_src)) {
        __CPROVER_assert(dst.tail.prev == old_src.tail.prev, "dst.tail.prev linked correctly");
        __CPROVER_assert(old_src.tail.prev->next == &dst.tail, "old_src.tail.prev->next points to dst.tail");

        if (!aws_linked_list_empty(&old_dst)) {
            __CPROVER_assert(old_dst.tail.prev->next == old_src.head.next, "old_dst.tail.prev->next links to old_src.head.next");
            __CPROVER_assert(old_src.head.next->prev == old_dst.tail.prev, "old_src.head.next->prev links to old_dst.tail.prev");
        } else {
            __CPROVER_assert(dst.head.next == old_src.head.next, "dst.head.next links to old_src.head.next");
            __CPROVER_assert(old_src.head.next->prev == &dst.head, "old_src.head.next->prev links to dst.head");
        }
    } else {
        __CPROVER_assert(dst.head.next == old_dst.head.next, "dst.head.next unchanged");
        __CPROVER_assert(dst.tail.prev == old_dst.tail.prev, "dst.tail.prev unchanged");
        if (!aws_linked_list_empty(&dst)) {
            __CPROVER_assert(dst.head.next->prev == &dst.head, "dst.head.next->prev correct");
            __CPROVER_assert(dst.tail.prev->next == &dst.tail, "dst.tail.prev->next correct");
        }
    }
}
