#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last = old_src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst remains valid");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src remains valid");

    __CPROVER_assert(src.head.next == &src.tail, "src head next points to tail");
    __CPROVER_assert(src.tail.prev == &src.head, "src tail prev points to head");

    if (old_src.head.next == &old_src.tail) {
        __CPROVER_assert(dst.head.next == old_dst.head.next, "dst head unchanged when src empty");
        __CPROVER_assert(dst.tail.prev == old_dst.tail.prev, "dst tail unchanged when src empty");
    } else {
        __CPROVER_assert(dst.head.next == old_dst.head.next, "dst head unchanged when src non-empty");
        __CPROVER_assert(dst.tail.prev == old_src_last, "dst tail points to last src node");
        __CPROVER_assert(old_dst_last->next == old_src_first, "old dst last links to old src first");
        __CPROVER_assert(old_src_first->prev == old_dst_last, "old src first links back to old dst last");
    }
}
