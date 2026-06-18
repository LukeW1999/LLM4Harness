#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
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

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Invariant: head.prev is always NULL and tail.next is always NULL */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Determine whether src was originally empty */
    if (old_src.head.next == &old_src.tail) {
        /* src was empty: dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        /* src remains empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* src was non‑empty: src must become empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst head.next unchanged */
        assert(dst.head.next == old_dst.head.next);
        /* dst tail.prev now points to the last node of the original src */
        assert(dst.tail.prev == old_src_last);
        /* The former last node of dst now links to the first node of src */
        assert(old_dst_last->next == old_src_first);
        /* The first node of src now links back to the former last node of dst */
        assert(old_src_first->prev == old_dst_last);
        /* The former last node of src now links to the new tail sentinel */
        assert(old_src_last->next == &dst.tail);
    }
}
