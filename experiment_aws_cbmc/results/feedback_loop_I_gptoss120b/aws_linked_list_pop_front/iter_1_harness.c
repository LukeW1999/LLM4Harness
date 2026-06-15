#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are initially valid */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_back = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_front = old_src.head.next;
    struct aws_linked_list_node *old_src_back = old_src.tail.prev;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Post‑condition: source list is empty */
    assert(aws_linked_list_empty(&src));

    /* Distinguish the two cases: source was empty or not */
    if (aws_linked_list_empty(&old_src)) {
        /* When source was empty, destination must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.head.prev == old_dst.head.prev);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.tail.next == old_dst.tail.next);
    } else {
        /* When source was non‑empty, destination gains the source nodes at the back */
        /* New tail points to the former last node of source */
        assert(dst.tail.prev == old_src_back);
        assert(dst.tail.prev->next == &dst.tail);

        /* The previous last node of destination now links to the former first node of source */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* Head of destination remains unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.head.prev == old_dst.head.prev);
    }
}
