#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound the source and destination lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* The two lists must be distinct */
    __CPROVER_assume(&src != &dst);

    /* Save old state for later comparison */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    /* Remember key pointers from the old state */
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;
    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* Post‑condition: source list must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(aws_linked_list_empty(&src));

    if (src_was_empty) {
        /* If source was empty, destination must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
    } else {
        /* Source was non‑empty: destination now contains old dst followed by old src */

        /* New tail predecessor is the former last node of src */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);
        assert(old_src_last->prev == (dst_was_empty ? &dst.head : old_dst_last));

        if (dst_was_empty) {
            /* Destination was empty, so its first node is the former first node of src */
            assert(dst.head.next == old_src_first);
            assert(old_src_first->prev == &dst.head);
        } else {
            /* Destination was non‑empty, so its original last node now links to src's first node */
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
            /* Destination's first node remains unchanged */
            assert(dst.head.next == old_dst_first);
        }
    }
}
