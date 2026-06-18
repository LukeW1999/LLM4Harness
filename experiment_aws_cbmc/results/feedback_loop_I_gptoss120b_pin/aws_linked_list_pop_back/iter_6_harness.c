#define AWS_STATIC_IMPL
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list src;
    struct aws_linked_list dst;

    aws_linked_list_init(&src);
    aws_linked_list_init(&dst);

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(&src != &dst);

    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *old_src_head = src.head;
    struct aws_linked_list_node *old_src_tail = src.tail;
    struct aws_linked_list_node *old_dst_head = dst.head;
    struct aws_linked_list_node *old_dst_tail = dst.tail;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    assert(src.head == NULL);
    assert(src.tail == NULL);

    if (old_src_head == NULL) {
        assert(dst.head == old_dst_head);
        assert(dst.tail == old_dst_tail);
    } else if (old_dst_head == NULL) {
        assert(dst.head == old_src_head);
        assert(dst.tail == old_src_tail);
    } else {
        assert(dst.head == old_dst_head);
        assert(dst.tail == old_src_tail);
        assert(old_dst_tail->next == old_src_head);
        assert(old_src_head->prev == old_dst_tail);
    }
}
