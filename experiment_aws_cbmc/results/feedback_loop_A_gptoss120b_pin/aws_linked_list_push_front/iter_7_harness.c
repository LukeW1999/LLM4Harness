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

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst valid after move");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src valid after move");

    bool src_was_empty = aws_linked_list_empty(&old_src);
    bool dst_was_empty = aws_linked_list_empty(&old_dst);

    if (src_was_empty) {
        __CPROVER_assert(dst.head == old_dst.head, "dst head unchanged when src empty");
        __CPROVER_assert(dst.tail == old_dst.tail, "dst tail unchanged when src empty");
        __CPROVER_assert(src.head == old_src.head, "src head unchanged when src empty");
        __CPROVER_assert(src.tail == old_src.tail, "src tail unchanged when src empty");
    } else {
        __CPROVER_assert(aws_linked_list_empty(&src), "src empty after move");

        if (dst_was_empty) {
            __CPROVER_assert(dst.head == old_src.head, "dst head becomes src head when dst empty");
            __CPROVER_assert(dst.tail == old_src.tail, "dst tail becomes src tail when dst empty");
        } else {
            __CPROVER_assert(dst.head == old_dst.head, "dst head unchanged when dst not empty");
            __CPROVER_assert(dst.tail == old_src.tail, "dst tail becomes src tail when dst not empty");
            __CPROVER_assert(old_dst.tail->next == old_src.head, "old dst tail next points to old src head");
            __CPROVER_assert(old_src.head->prev == old_dst.tail, "old src head prev points to old dst tail");
        }
    }
}
