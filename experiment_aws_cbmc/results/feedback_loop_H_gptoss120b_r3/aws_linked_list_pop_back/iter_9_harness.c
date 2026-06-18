#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
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

    struct aws_linked_list_node *old_src_front = NULL;
    struct aws_linked_list_node *old_src_back = NULL;
    if (!aws_linked_list_empty(&src)) {
        old_src_front = src.head.next;
        old_src_back = src.tail.prev;
    }

    struct aws_linked_list_node *old_dst_back = NULL;
    if (!aws_linked_list_empty(&dst)) {
        old_dst_back = dst.tail.prev;
    }

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_empty(&src), "src should be empty");
    __CPROVER_assert(src.head.next == &src.tail, "src head next points to tail");
    __CPROVER_assert(src.tail.prev == &src.head, "src tail prev points to head");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src remains valid");

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst remains valid");

    __CPROVER_assert(dst.head.prev == NULL, "dst head prev is NULL");
    __CPROVER_assert(dst.tail.next == NULL, "dst tail next is NULL");
    __CPROVER_assert(src.head.prev == NULL, "src head prev is NULL");
    __CPROVER_assert(src.tail.next == NULL, "src tail next is NULL");

    if (old_src_front == NULL) {
        __CPROVER_assert(dst.head.next == old_dst.head.next, "dst head next unchanged");
        __CPROVER_assert(dst.tail.prev == old_dst.tail.prev, "dst tail prev unchanged");
    } else {
        __CPROVER_assert(dst.tail.prev == old_src_back, "dst tail prev points to old src back");
        __CPROVER_assert(old_src_back->next == &dst.tail, "old src back next points to dst tail");
        if (old_dst_back != NULL) {
            __CPROVER_assert(old_dst_back->next == old_src_front, "old dst back next points to old src front");
            __CPROVER_assert(old_src_front->prev == old_dst_back, "old src front prev points to old dst back");
        } else {
            __CPROVER_assert(dst.head.next == old_src_front, "dst head next points to old src front");
            __CPROVER_assert(old_src_front->prev == &dst.head, "old src front prev points to dst head");
        }
    }
}
