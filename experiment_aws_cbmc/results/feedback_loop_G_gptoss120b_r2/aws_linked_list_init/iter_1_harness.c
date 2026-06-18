#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Maximum number of nodes that can be allocated in a list for the proof.
 * This macro is defined in the Makefile used by the verification harnesses. */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* 2. Save old copies for post‑condition checks */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑conditions that must always hold */

    /* Both lists remain valid */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* The sentinel nodes of each list keep their invariant fields unchanged */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    /* After the move, src must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* If src was empty before the call, dst must be unchanged */
    if (old_src.head.next == &old_src.tail) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src was non‑empty: its nodes are now appended to dst */

        /* Capture old boundary nodes */
        struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;   /* may be &old_dst.head if dst empty */
        struct aws_linked_list_node *old_src_first = old_src.head.next;
        struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

        /* New tail of dst is the former last node of src */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);

        /* The node that used to be the last of dst now points to the first of src */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    }
}
