#include <assert.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEMS 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, allocator, MAX_ITEMS);
    ensure_linked_list_is_allocated(&src, allocator, MAX_ITEMS);

    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last = src.tail.prev;

    bool old_src_empty = (src.head.next == &src.tail);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    if (old_src_empty) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        assert(dst.tail.prev == old_src_last);
        assert(dst.tail.prev->next == &dst.tail);
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
        assert(dst.head.next == old_dst.head.next);
    }

    assert(dst.head.prev == ((void *)0));
    assert(src.head.prev == ((void *)0));
    assert(dst.tail.next == ((void *)0));
    assert(src.tail.next == ((void *)0));
}
