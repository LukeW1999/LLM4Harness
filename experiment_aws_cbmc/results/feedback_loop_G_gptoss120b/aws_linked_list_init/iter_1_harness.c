#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src); /* dst and src must be distinct */

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Postconditions */

    /* Both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* dst structural invariants that never change */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    if (old_src.head.next == &old_src.tail) {
        /* src was empty: dst must be unchanged */
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
    } else {
        /* src was non‑empty: dst now contains old dst followed by old src */

        /* The first element of dst remains the old first element */
        assert(dst.head.next == old_dst_first);

        /* The last element of dst is the old last element of src */
        assert(dst.tail.prev == old_src_last);

        /* Linkage between old dst last and old src first */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);

        /* Tail linkage */
        assert(old_src_last->next == &dst.tail);
        assert(dst.tail.prev->prev == old_src_last);
    }

    /* Ensure that the head and tail sentinel nodes themselves are unchanged */
    assert(dst.head.next != NULL);
    assert(dst.tail.prev != NULL);
}
