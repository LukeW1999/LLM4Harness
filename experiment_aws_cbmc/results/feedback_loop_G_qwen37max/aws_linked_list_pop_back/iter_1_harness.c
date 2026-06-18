#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_front_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    
    bool src_was_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *old_dst_front = dst.head.next;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    aws_linked_list_move_all_front(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    if (!src_was_empty) {
        assert(dst.head.next == old_src_front);
        assert(old_src_front->prev == &dst.head);
        assert(old_src_back->next == old_dst_front);
        assert(old_dst_front->prev == old_src_back);
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.head.prev == old_dst.head.prev);
        assert(dst.tail.next == old_dst.tail.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    }
}
