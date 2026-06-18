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

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    bool src_was_empty = aws_linked_list_empty(&old_src);
    bool dst_was_empty = aws_linked_list_empty(&old_dst);

    if (src_was_empty) {
        /* No nodes moved; both lists unchanged */
        assert(dst.head == old_dst.head);
        assert(dst.tail == old_dst.tail);
        assert(src.head == old_src.head);
        assert(src.tail == old_src.tail);
    } else {
        /* src should be empty after move */
        assert(aws_linked_list_empty(&src));

        if (dst_was_empty) {
            /* dst was empty, now contains former src */
            assert(dst.head == old_src.head);
            assert(dst.tail == old_src.tail);
        } else {
            /* dst not empty, head unchanged, tail becomes former src tail */
            assert(dst.head == old_dst.head);
            assert(dst.tail == old_src.tail);
            /* Verify linking between old dst tail and old src head */
            assert(old_dst.tail->next == old_src.head);
            assert(old_src.head->prev == old_dst.tail);
        }
    }
}
