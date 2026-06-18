#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    bool src_was_empty = (old_src.head.next == &old_src.tail);
    bool dst_was_empty = (old_dst.head.next == &old_dst.tail);

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&src), "src valid after move");
    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst valid after move");

    __CPROVER_assert(src.head.next == &src.tail, "src empty head.next");
    __CPROVER_assert(src.tail.prev == &src.head, "src empty tail.prev");
    __CPROVER_assert(src.head.prev == ((void *)0), "src head.prev null");
    __CPROVER_assert(src.tail.next == ((void *)0), "src tail.next null");

    if (src_was_empty) {
        __CPROVER_assert(dst.head.next == old_dst.head.next, "dst unchanged head.next");
        __CPROVER_assert(dst.tail.prev == old_dst.tail.prev, "dst unchanged tail.prev");
    } else {
        __CPROVER_assert(dst.tail.prev == old_src.tail.prev, "dst tail.prev updated");
        __CPROVER_assert(old_src.tail.prev->next == &dst.tail, "old src tail.prev links to dst.tail");

        if (dst_was_empty) {
            __CPROVER_assert(dst.head.next == old_src.head.next, "dst head.next from src");
            __CPROVER_assert(old_src.head.next->prev == &dst.head, "src first node prev points to dst.head");
        } else {
            __CPROVER_assert(dst.head.next == old_dst.head.next, "dst head.next unchanged");
            __CPROVER_assert(old_dst.tail.prev->next == old_src.head.next, "old dst tail links to src first");
            __CPROVER_assert(old_src.head.next->prev == old_dst.tail.prev, "src first prev points to old dst tail");
        }
    }

    __CPROVER_assert(src.head.prev == ((void *)0), "src head.prev invariant");
    __CPROVER_assert(src.tail.next == ((void *)0), "src tail.next invariant");
    __CPROVER_assert(dst.head.prev == ((void *)0), "dst head.prev invariant");
    __CPROVER_assert(dst.tail.next == ((void *)0), "dst tail.next invariant");
}
