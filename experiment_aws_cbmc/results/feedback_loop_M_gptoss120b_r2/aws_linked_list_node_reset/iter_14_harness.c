#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑conditions */
    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst must be valid");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src must be valid");

    /* src must be empty */
    __CPROVER_assert(src.head == NULL, "src.head is NULL");
    __CPROVER_assert(src.tail == NULL, "src.tail is NULL");

    if (old_src.head == NULL) {
        /* src was empty, dst unchanged */
        __CPROVER_assert(dst.head == old_dst.head, "dst.head unchanged when src empty");
        __CPROVER_assert(dst.tail == old_dst.tail, "dst.tail unchanged when src empty");
    } else {
        /* src contributed nodes */
        if (old_dst.head == NULL) {
            /* dst was empty, now equals old src */
            __CPROVER_assert(dst.head == old_src.head, "dst.head equals old src.head when dst empty");
        } else {
            /* dst non‑empty, head unchanged */
            __CPROVER_assert(dst.head == old_dst.head, "dst.head unchanged when both non‑empty");
            /* linkage between old dst tail and old src head */
            struct aws_linked_list_node *old_dst_tail = old_dst.tail;
            struct aws_linked_list_node *old_src_head = old_src.head;
            __CPROVER_assert(old_dst_tail->next == old_src_head, "old dst tail linked to old src head");
            __CPROVER_assert(old_src_head->prev == old_dst_tail, "old src head linked back to old dst tail");
        }
        /* tail must be old src tail */
        __CPROVER_assert(dst.tail == old_src.tail, "dst.tail equals old src.tail");
    }
}
