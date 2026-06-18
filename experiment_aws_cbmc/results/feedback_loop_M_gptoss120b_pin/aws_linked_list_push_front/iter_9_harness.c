#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list src;
    struct aws_linked_list dst;

    aws_linked_list_init(&src);
    aws_linked_list_init(&dst);

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(&src != &dst);

    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *src_first = src.head.next;
    struct aws_linked_list_node *src_last  = src.tail.prev;

    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    if (!src_was_empty) {
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        assert(dst.tail.prev == src_last);
        assert(src_last->next == &dst.tail);

        if (!dst_was_empty) {
            assert(old_dst.tail.prev->next == src_first);
            assert(src_first->prev == old_dst.tail.prev);
        } else {
            assert(dst.head.next == src_first);
            assert(src_first->prev == &dst.head);
        }
    } else {
        assert(src.head.next == old_src.head.next);
        assert(src.tail.prev == old_src.tail.prev);

        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    }
}
