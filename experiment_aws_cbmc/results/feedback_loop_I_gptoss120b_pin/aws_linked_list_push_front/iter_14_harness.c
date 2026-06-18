#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list_node *old_dst_head = dst.head;
    struct aws_linked_list_node *old_dst_tail = dst.tail;
    struct aws_linked_list_node *old_src_head = src.head;
    struct aws_linked_list_node *old_src_tail = src.tail;

    bool src_was_empty = (old_src_head == NULL);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    assert(src.head == NULL);
    assert(src.tail == NULL);

    if (src_was_empty) {
        assert(dst.head == old_dst_head);
        assert(dst.tail == old_dst_tail);
    } else {
        assert(dst.tail == old_src_tail);
        if (old_dst_tail != NULL) {
            assert(old_dst_tail->next == old_src_head);
            assert(old_src_head->prev == old_dst_tail);
        } else {
            assert(dst.head == old_src_head);
        }
        if (old_dst_head != NULL) {
            assert(dst.head == old_dst_head);
        }
    }
}
