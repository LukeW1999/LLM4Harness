#include <assert.h>
#include <stdbool.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
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

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_head = dst.head;
    struct aws_linked_list_node *old_dst_tail = dst.tail;
    struct aws_linked_list_node *old_src_head = src.head;
    struct aws_linked_list_node *old_src_tail = src.tail;

    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head == NULL);
    assert(src.tail == NULL);

    if (src_was_empty) {
        /* dst should be unchanged */
        assert(dst.head == old_dst_head);
        assert(dst.tail == old_dst_tail);
    } else {
        /* dst.tail must be the old src.tail */
        assert(dst.tail == old_src_tail);

        if (dst_was_empty) {
            /* when dst was empty, its head becomes old src.head */
            assert(dst.head == old_src_head);
        } else {
            /* when dst was non‑empty, its head stays the same */
            assert(dst.head == old_dst_head);
            /* the previous tail of dst should now link to the old src head */
            assert(old_dst_tail->next == old_src_head);
            assert(old_src_head->prev == old_dst_tail);
        }
    }
}
