#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old pointers for later checks */
    struct aws_linked_list_node *old_dst_head_next = dst.head.next;
    struct aws_linked_list_node *old_dst_tail_prev = dst.tail.prev;
    struct aws_linked_list_node *old_src_head_next = src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = src.tail.prev;

    bool old_src_empty = (old_src_head_next == &src.tail);
    bool old_dst_empty = (old_dst_head_next == &dst.tail);

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst is valid after move");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src is valid after move");
    __CPROVER_assert(aws_linked_list_empty(&src), "src is empty after move");

    if (!old_src_empty) {
        /* The tail of dst should now point to the former last element of src */
        __CPROVER_assert(dst.tail.prev == old_src_tail_prev,
                         "dst.tail.prev linked to old src tail");
        __CPROVER_assert(old_src_tail_prev->next == &dst.tail,
                         "old src tail next points to dst.tail");

        if (!old_dst_empty) {
            /* The former last element of dst should link to the first element of src */
            __CPROVER_assert(old_dst_tail_prev->next == old_src_head_next,
                             "old dst tail next links to old src head next");
            __CPROVER_assert(old_src_head_next->prev == old_dst_tail_prev,
                             "old src head next prev links to old dst tail");
        } else {
            /* dst was empty, so its head should now point to the first element of src */
            __CPROVER_assert(dst.head.next == old_src_head_next,
                             "dst.head.next links to old src head next");
            __CPROVER_assert(old_src_head_next->prev == &dst.head,
                             "old src head next prev links to dst.head");
        }
    } else {
        /* src was empty, dst should be unchanged */
        __CPROVER_assert(dst.head.next == old_dst_head_next,
                         "dst.head.next unchanged");
        __CPROVER_assert(dst.tail.prev == old_dst_tail_prev,
                         "dst.tail.prev unchanged");
        if (!aws_linked_list_empty(&dst)) {
            __CPROVER_assert(dst.head.next->prev == &dst.head,
                             "dst.head.next->prev correct");
            __CPROVER_assert(dst.tail.prev->next == &dst.tail,
                             "dst.tail.prev->next correct");
        }
    }
}
