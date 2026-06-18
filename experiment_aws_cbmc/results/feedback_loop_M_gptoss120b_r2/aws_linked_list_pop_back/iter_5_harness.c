#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list src;
    struct aws_linked_list dst;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    assert(aws_linked_list_empty(&src));

    if (!aws_linked_list_empty(&src)) {
        assert(src.head->prev == NULL);
        assert(src.tail->next == NULL);
    } else {
        assert(src.head == NULL);
        assert(src.tail == NULL);
    }

    if (!aws_linked_list_empty(&dst)) {
        assert(dst.head->prev == NULL);
        assert(dst.tail->next == NULL);
    } else {
        assert(dst.head == NULL);
        assert(dst.tail == NULL);
    }

    if (aws_linked_list_empty(&old_src)) {
        assert(dst.head == old_dst.head);
        assert(dst.tail == old_dst.tail);
    } else {
        if (aws_linked_list_empty(&old_dst)) {
            assert(dst.head == old_src.head);
            assert(dst.tail == old_src.tail);
        } else {
            assert(dst.head == old_dst.head);
            assert(dst.tail == old_src.tail);
            assert(old_dst.tail->next == old_src.head);
            assert(old_src.head->prev == old_dst.tail);
        }
    }
}
