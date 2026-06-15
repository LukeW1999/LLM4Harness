#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = dst.head.next;
    struct aws_linked_list_node *old_dst_last  = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    if (old_src_first == &old_src.tail) {
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
    } else {
        if (old_dst_first == &old_dst.tail) {
            assert(dst.head.next == old_src_first);
            assert(dst.tail.prev == old_src_last);
            assert(old_src_first->prev == &dst.head);
            assert(old_src_last->next == &dst.tail);
        } else {
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
            assert(dst.tail.prev == old_src_last);
            assert(old_src_last->next == &dst.tail);
        }
    }

    struct aws_linked_list_node *cur = dst.head.next;
    while (cur != &dst.tail) {
        bool in_old_dst = false;
        struct aws_linked_list_node *tmp = old_dst.head.next;
        while (tmp != &old_dst.tail) {
            if (cur == tmp) {
                in_old_dst = true;
                break;
            }
            tmp = tmp->next;
        }
        bool in_old_src = false;
        struct aws_linked_list_node *tmp2 = old_src.head.next;
        while (tmp2 != &old_src.tail) {
            if (cur == tmp2) {
                in_old_src = true;
                break;
            }
            tmp2 = tmp2->next;
        }
        assert(in_old_dst || in_old_src);
        cur = cur->next;
    }
}
