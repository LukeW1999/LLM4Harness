#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    ensure_linked_list_is_allocated(&dst, allocator, 5);
    ensure_linked_list_is_allocated(&src, allocator, 5);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_back = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_front = old_src.head.next;
    struct aws_linked_list_node *old_src_back = old_src.tail.prev;

    bool src_was_empty = (old_src.head.next == &old_src.tail);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (src_was_empty) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);
    } else {
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
        assert(src.head.prev == NULL);
        assert(src.tail.next == NULL);

        assert(dst.tail.prev == old_src_back);
        assert(dst.tail.prev->next == &dst.tail);
        assert(old_src_back->prev == old_src_back->prev);

        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
