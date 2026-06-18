#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

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

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst remains valid");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src remains valid");

    __CPROVER_assert(src.head.next == &src.tail, "src head points to tail");
    __CPROVER_assert(src.tail.prev == &src.head, "src tail points to head");

    if (old_src_head_next == &old_src.tail) {
        __CPROVER_assert(dst.head.next == old_dst_head_next, "dst head unchanged");
        __CPROVER_assert(dst.tail.prev == old_dst_tail_prev, "dst tail unchanged");
    } else {
        __CPROVER_assert(dst.tail.prev == old_src_tail_prev, "dst tail updated to old src tail");

        if (old_dst_tail_prev != &old_dst.tail) {
            __CPROVER_assert(old_dst_tail_prev->next == old_src_head_next,
                             "old dst tail links to old src head");
        } else {
            __CPROVER_assert(dst.head.next == old_src_head_next,
                             "dst empty now points to old src head");
        }

        if (old_dst_tail_prev != &old_dst.tail) {
            __CPROVER_assert(old_src_head_next->prev == old_dst_tail_prev,
                             "old src head prev points to old dst tail");
        } else {
            __CPROVER_assert(old_src_head_next->prev == &dst.head,
                             "old src head prev points to dst head");
        }
    }

    __CPROVER_assert(dst.head.prev == ((void *)0), "dst head.prev is NULL");
    __CPROVER_assert(dst.tail.next == ((void *)0), "dst tail.next is NULL");
    __CPROVER_assert(src.head.prev == ((void *)0), "src head.prev is NULL");
    __CPROVER_assert(src.tail.next == ((void *)0), "src tail.next is NULL");
}
