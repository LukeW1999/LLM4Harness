#include <stdbool.h>
#include <assert.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    struct aws_linked_list_node dst_node;
    struct aws_linked_list_node src_node;
    
    aws_linked_list_push_back(&dst, &dst_node);
    aws_linked_list_push_back(&src, &src_node);

    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    assert(old_dst_back->next == old_src_front);
    assert(old_src_front->prev == old_dst_back);
    assert(old_src_back->next == &dst.tail);
    assert(dst.tail.prev == old_src_back);
}
