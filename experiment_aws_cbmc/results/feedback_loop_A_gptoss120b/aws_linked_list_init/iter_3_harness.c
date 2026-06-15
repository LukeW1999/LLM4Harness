#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    /* Allocate and possibly populate the lists */
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preserve the original state for later checks */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_tail = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_head = old_src.head.next;
    struct aws_linked_list_node *old_src_tail = old_src.tail.prev;

    bool src_was_empty = (old_src_head == &old_src.tail);

    /* Exercise the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Verify post‑conditions */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Invariant list structure */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        /* When src was empty, dst should be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* src becomes empty and its nodes are appended to dst */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
        assert(dst.tail.prev == old_src_tail);
        assert(old_dst_tail->next == old_src_head);
        assert(old_src_head->prev == old_dst_tail);
        assert(dst.head.next == old_dst.head.next);
    }
}

int main(void) {
    aws_linked_list_move_all_back_harness();
    return 0;
}
