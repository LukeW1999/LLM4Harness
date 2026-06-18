#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    assert(dst.head->prev == NULL);
    assert(dst.tail->next == NULL);
    assert(src.head->prev == NULL);
    assert(src.tail->next == NULL);

    assert(src.head->next == src.tail);
    assert(src.tail->prev == src.head);

    if (old_src.head->next == old_src.tail) {
        assert(dst.head->next == old_dst.head->next);
        assert(dst.tail->prev == old_dst.tail->prev);
    } else {
        assert(dst.tail->prev == old_src.tail->prev);
        if (old_dst.head->next != old_dst.tail) {
            struct aws_linked_list_node *old_last_dst = old_dst.tail->prev;
            assert(old_last_dst->next == old_src.head->next);
            assert(old_src.head->next->prev == old_last_dst);
        } else {
            assert(dst.head->next == old_src.head->next);
        }
    }
}
