#include <assert.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

struct test_node {
    struct aws_linked_list_node node;
    int data;
};

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    const size_t max_items = 5;
    struct test_node src_nodes[5];
    size_t i;
    for (i = 0; i < max_items; ++i) {
        __CPROVER_assume(i == 0 || __CPROVER_nondet_bool());
        if (__CPROVER_nondet_bool()) {
            aws_linked_list_push_back(&src, &src_nodes[i].node);
        } else {
            break;
        }
    }

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last = src.tail.prev;
    bool src_was_empty = aws_linked_list_empty(&src);

    aws_linked_list_move_all_back(&dst, &src);

    if (src_was_empty) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
        assert(dst.tail.prev == old_src_last);
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
        assert(dst.head.next == old_dst.head.next);
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
