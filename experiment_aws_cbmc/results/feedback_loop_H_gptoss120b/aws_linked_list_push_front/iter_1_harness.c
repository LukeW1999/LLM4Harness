#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;
    bool src_was_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *old_dst_last = dst.tail.prev;
    struct aws_linked_list_node *old_src_last = src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Postconditions */

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        /* When src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);
    } else {
        /* When src was non‑empty, dst's new last node is the former src last node */
        assert(dst.tail.prev == old_src_last);
        /* The node before the new last (old dst last) should still be linked correctly */
        assert(old_dst_last->next == old_src_last);
        assert(old_src_last->prev == old_dst_last);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);
    }
}
