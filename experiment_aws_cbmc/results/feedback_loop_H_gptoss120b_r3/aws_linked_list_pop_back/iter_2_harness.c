#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_src_front = NULL;
    struct aws_linked_list_node *old_src_back  = NULL;
    if (!aws_linked_list_empty(&src)) {
        old_src_front = src.head.next;
        old_src_back  = src.tail.prev;
    }

    struct aws_linked_list_node *old_dst_back = NULL;
    if (!aws_linked_list_empty(&dst)) {
        old_dst_back = dst.tail.prev;
    }

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postconditions for src: must be empty */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(aws_linked_list_is_valid(&src));

    /* Postconditions for dst: must be valid */
    assert(aws_linked_list_is_valid(&dst));

    /* Unchanged structural invariants */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* If src was originally empty, dst must be unchanged */
    if (old_src_front == NULL) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src was non‑empty: verify splice */
        /* New tail of dst is the old src back */
        assert(dst.tail.prev == old_src_back);
        /* Old src back now points to dst tail */
        assert(old_src_back->next == &dst.tail);
        /* Old src front now points back to the previous dst back (or head if dst was empty) */
        if (old_dst_back != NULL) {
            assert(old_dst_back->next == old_src_front);
            assert(old_src_front->prev == old_dst_back);
        } else {
            /* dst was empty before splice */
            assert(dst.head.next == old_src_front);
            assert(old_src_front->prev == &dst.head);
        }
    }
}
