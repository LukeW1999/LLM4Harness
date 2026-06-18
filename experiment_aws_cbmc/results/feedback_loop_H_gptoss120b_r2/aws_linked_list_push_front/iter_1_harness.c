#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* The function under test */
void aws_linked_list_move_all_back(struct aws_linked_list *AWS_RESTRICT dst,
                                   struct aws_linked_list *AWS_RESTRICT src);

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* The two lists must be distinct */
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition assertions */

    /* Both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(aws_linked_list_empty(&src));

    if (!aws_linked_list_empty(&old_src)) {
        /* src was non‑empty, its elements are now at the back of dst */

        /* The last node of dst should be the former last node of src */
        assert(dst.tail.prev == old_src.tail.prev);
        /* That node must point to dst's tail */
        assert(old_src.tail.prev->next == &dst.tail);

        if (!aws_linked_list_empty(&old_dst)) {
            /* dst was non‑empty: its former last node should now point to src's first node */
            assert(old_dst.tail.prev->next == old_src.head.next);
            /* src's first node should point back to the former last node of dst */
            assert(old_src.head.next->prev == old_dst.tail.prev);
        } else {
            /* dst was empty: its head should now point directly to src's first node */
            assert(dst.head.next == old_src.head.next);
            assert(old_src.head.next->prev == &dst.head);
        }
    } else {
        /* src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        if (!aws_linked_list_empty(&dst)) {
            /* ensure bidirectional links are still correct for the unchanged nodes */
            assert(dst.head.next->prev == &dst.head);
            assert(dst.tail.prev->next == &dst.tail);
        }
    }
}
