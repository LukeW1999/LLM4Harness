#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    aws_linked_list_init(&dst);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    struct aws_linked_list src;
    aws_linked_list_init(&src);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    size_t dst_len = nondet_uint8_t() % (MAX_LINKED_LIST_ITEM_ALLOCATION + 1);
    size_t src_len = nondet_uint8_t() % (MAX_LINKED_LIST_ITEM_ALLOCATION + 1);

    struct aws_linked_list_node dst_nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];
    for (size_t i = 0; i < dst_len; ++i) {
        aws_linked_list_push_back(&dst, &dst_nodes[i]);
        __CPROVER_assume(aws_linked_list_is_valid(&dst));
    }

    struct aws_linked_list_node src_nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];
    for (size_t i = 0; i < src_len; ++i) {
        aws_linked_list_push_back(&src, &src_nodes[i]);
        __CPROVER_assume(aws_linked_list_is_valid(&src));
    }

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (!aws_linked_list_empty(&old_src)) {
        assert(dst.tail.prev == old_src_last);
        if (!aws_linked_list_empty(&old_dst)) {
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
        } else {
            assert(dst.head.next == old_src_first);
            assert(old_src_first->prev == &dst.head);
        }
    } else {
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
        if (!aws_linked_list_empty(&dst)) {
            assert(old_dst_first->prev == &dst.head);
            assert(old_dst_last->next == &dst.tail);
        } else {
            assert(dst.head.next == &dst.tail);
            assert(dst.tail.prev == &dst.head);
        }
    }
}
