#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list src;
    struct aws_linked_list dst;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&src), "src valid after move");
    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst valid after move");

    __CPROVER_assert(aws_linked_list_empty(&src), "src empty after move");

    if (!aws_linked_list_empty(&src)) {
        __CPROVER_assert(src.head->prev == ((void *)0), "src head prev null");
        __CPROVER_assert(src.tail->next == ((void *)0), "src tail next null");
    } else {
        __CPROVER_assert(src.head == ((void *)0), "src head null");
        __CPROVER_assert(src.tail == ((void *)0), "src tail null");
    }

    if (!aws_linked_list_empty(&dst)) {
        __CPROVER_assert(dst.head->prev == ((void *)0), "dst head prev null");
        __CPROVER_assert(dst.tail->next == ((void *)0), "dst tail next null");
    } else {
        __CPROVER_assert(dst.head == ((void *)0), "dst head null");
        __CPROVER_assert(dst.tail == ((void *)0), "dst tail null");
    }

    if (aws_linked_list_empty(&old_src)) {
        __CPROVER_assert(dst.head == old_dst.head, "dst head unchanged when src empty");
        __CPROVER_assert(dst.tail == old_dst.tail, "dst tail unchanged when src empty");
    } else {
        if (aws_linked_list_empty(&old_dst)) {
            __CPROVER_assert(dst.head == old_src.head, "dst head from src when dst empty");
            __CPROVER_assert(dst.tail == old_src.tail, "dst tail from src when dst empty");
        } else {
            __CPROVER_assert(dst.head == old_dst.head, "dst head unchanged when both non-empty");
            __CPROVER_assert(dst.tail == old_src.tail, "dst tail from src when both non-empty");
            __CPROVER_assert(old_dst.tail->next == old_src.head, "old dst tail linked to old src head");
            __CPROVER_assert(old_src.head->prev == old_dst.tail, "old src head linked back to old dst tail");
        }
    }
}
