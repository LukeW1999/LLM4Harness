#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* 1. Declare and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = dst.tail.prev;   /* may be &dst.head if empty */
    struct aws_linked_list_node *old_src_first = src.head.next; /* may be &src.tail if empty */
    struct aws_linked_list_node *old_src_last  = src.tail.prev; /* may be &src.head if empty */

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Postconditions */

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* dst invariants for head and tail */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    /* If src was originally empty, dst must be unchanged */
    if (old_src_first == &old_src.tail) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src was non‑empty: dst now ends with the former src nodes */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);

        /* The former last node of dst (or head if dst was empty) now links to the first src node */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    }
}
