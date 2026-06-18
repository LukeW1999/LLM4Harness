#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

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

    struct aws_linked_list_node *old_dst_last = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last = src.tail.prev;
    _Bool src_was_empty = aws_linked_list_empty(&src);

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst must be valid");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src must be valid");

    __CPROVER_assert(aws_linked_list_empty(&src), "src must be empty");
    __CPROVER_assert(src.head.next == &src.tail, "src head next points to tail");
    __CPROVER_assert(src.tail.prev == &src.head, "src tail prev points to head");

    __CPROVER_assert(dst.head.prev == ((void *)0), "dst head prev is NULL");
    __CPROVER_assert(dst.tail.next == ((void *)0), "dst tail next is NULL");
    __CPROVER_assert(src.head.prev == ((void *)0), "src head prev is NULL");
    __CPROVER_assert(src.tail.next == ((void *)0), "src tail next is NULL");
    __CPROVER_assert(dst.head.next == old_dst.head.next, "dst front unchanged");

    if (src_was_empty) {
        __CPROVER_assert(dst.tail.prev == old_dst.tail.prev, "dst unchanged when src empty");
    } else {
        __CPROVER_assert(dst.tail.prev == old_src_last, "dst tail prev is old src last");
        __CPROVER_assert(old_dst_last->next == old_src_first, "old dst last links to old src first");
        __CPROVER_assert(old_src_last->next == &dst.tail, "old src last links to dst tail");
    }
}
