#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Pre‑conditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_tail = dst.tail.prev;
    struct aws_linked_list_node *old_src_tail = src.tail.prev;
    struct aws_linked_list_node *old_src_head_next = src.head.next;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑conditions */

    /* Both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Source list must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Destination list head should be unchanged */
    assert(dst.head.next == old_dst.head.next);

    /* Destination tail depends on whether source was empty */
    if (old_src_head_next == &old_src.tail) {
        /* Source was empty: destination unchanged */
        assert(dst.tail.prev == old_dst_tail);
    } else {
        /* Source non‑empty: new tail is the old source tail */
        assert(dst.tail.prev == old_src_tail);
    }
}
