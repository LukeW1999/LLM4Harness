#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = dst.head.next;
    struct aws_linked_list_node *old_dst_last  = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;

    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Postconditions */

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Head and tail sentinel pointers must remain unchanged */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        /* When src is empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        /* src remains empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* src becomes empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst now ends with the former last node of src */
        assert(dst.tail.prev == old_src_last);

        /* The node that was previously the last of dst now points to the first of src */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);

        /* The former last node of src now points to dst's tail sentinel */
        assert(old_src_last->next == &dst.tail);
        assert(dst.tail.prev->prev == old_src_last);
    }

    /* Unchanged fields for dst when src was non‑empty */
    if (!src_was_empty) {
        /* The first element of dst remains the same */
        assert(dst.head.next == old_dst_first);
    }

    /* Unchanged fields for src when it was non‑empty (now empty) */
    if (!src_was_empty) {
        /* No nodes other than sentinels remain reachable from src */
        /* (implicitly checked by validity and emptiness assertions) */
    }
}
