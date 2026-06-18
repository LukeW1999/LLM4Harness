#include <assert.h>
#include <stdbool.h>
#include <aws/common/allocator.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
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

    bool src_was_empty = aws_linked_list_empty(&src);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    assert(dst.head.next == old_dst.head.next);

    if (src_was_empty) {
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        assert(dst.tail.prev == old_src.tail.prev);
        assert(old_dst.tail.prev->next == old_src.head.next);
        assert(old_src.head.next->prev == old_dst.tail.prev);
        assert(old_src.tail.prev->next == &dst.tail);
    }
}
