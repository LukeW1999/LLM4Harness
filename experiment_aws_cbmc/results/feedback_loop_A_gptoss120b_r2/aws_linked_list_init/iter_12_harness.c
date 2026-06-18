#include <stdbool.h>
#include <assert.h>
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

    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_tail = dst.tail.prev;
    struct aws_linked_list_node *old_dst_head_next = dst.head.next;

    struct aws_linked_list_node *old_src_head = src.head.next;
    struct aws_linked_list_node *old_src_tail = src.tail.prev;

    bool src_was_empty = aws_linked_list_is_empty(&src);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        assert(dst.head.next == old_dst_head_next);
        assert(dst.tail.prev == old_dst_tail);
    } else {
        assert(dst.tail.prev == old_src_tail);
        assert(old_dst_tail->next == old_src_head);
        assert(old_src_head->prev == old_dst_tail);
    }

    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
}
