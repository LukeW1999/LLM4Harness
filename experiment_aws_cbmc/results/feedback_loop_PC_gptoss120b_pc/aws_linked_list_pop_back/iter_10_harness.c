#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <aws/common/allocator.h>
#include <aws/common/linked_list.h>
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

    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    bool src_was_empty = aws_linked_list_empty(&src);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (src_was_empty) {
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
        if (old_dst_first != &dst.tail) {
            assert(old_dst_first->prev == &dst.head);
        }
        if (old_dst_last != &dst.head) {
            assert(old_dst_last->next == &dst.tail);
        }
    } else {
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_src_last);
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
