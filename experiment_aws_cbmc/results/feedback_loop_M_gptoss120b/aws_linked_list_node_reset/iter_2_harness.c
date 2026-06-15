#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound destination list */
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Allocate and bound source list */
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;

    bool src_was_empty = aws_linked_list_empty(&src);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Source list must be empty after the operation */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (src_was_empty) {
        /* Destination list unchanged when source was empty */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* Destination list now ends with the former source last node */
        assert(dst.tail.prev == old_src_last);
        /* Head of destination unchanged */
        assert(dst.head.next == old_dst.head.next);
        /* Linkage between old destination last and old source first */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
        /* Source last now points to destination tail */
        assert(old_src_last->next == &dst.tail);
        /* Destination tail's prev points to source last (already asserted) */
        assert(dst.tail.prev == old_src_last);
    }
}
