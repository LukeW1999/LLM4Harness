#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src); /* distinct objects */

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_tail = dst.tail.prev;
    struct aws_linked_list_node *old_dst_head_next = dst.head.next;

    struct aws_linked_list_node *old_src_head = src.head.next;
    struct aws_linked_list_node *old_src_tail = src.tail.prev;

    bool src_was_empty = (old_src.head.next == &old_src.tail);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Unchanged structural fields */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        /* When source is empty, destination must be unchanged */
        assert(dst.head.next == old_dst_head_next);
        assert(dst.tail.prev == old_dst_tail);
    } else {
        /* Destination now ends with the former source tail */
        assert(dst.tail.prev == old_src_tail);
        /* The former destination tail now links to the former source head */
        assert(old_dst_tail->next == old_src_head);
        assert(old_src_head->prev == old_dst_tail);
    }

    /* Source must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
}
