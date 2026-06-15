#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound two linked lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(src.head.prev == NULL);
    __CPROVER_assume(dst.head.prev == NULL);
    __CPROVER_assume(src.tail.next == NULL);
    __CPROVER_assume(dst.tail.next == NULL);
    __CPROVER_assume(src.head.next != NULL);
    __CPROVER_assume(dst.head.next != NULL);
    __CPROVER_assume(src.tail.prev != NULL);
    __CPROVER_assume(dst.tail.prev != NULL);
    __CPROVER_assume(src.head.next != &src.tail); /* allow non‑empty */
    __CPROVER_assume(dst.head.next != &dst.tail); /* allow non‑empty */

    /* 2. Save old state */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    bool src_was_empty = aws_linked_list_empty(&old_src);
    bool dst_was_empty = aws_linked_list_empty(&old_dst);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: validity invariants */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* 5. Post‑condition: src must be empty */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 6. Post‑condition: dst changes depending on whether src was empty */
    if (src_was_empty) {
        /* src empty → dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src non‑empty → dst.tail.prev becomes old src.tail.prev */
        assert(dst.tail.prev == old_src.tail.prev);
        /* the new tail's next must point to the list tail sentinel */
        assert(dst.tail.prev->next == &dst.tail);
        /* the node preceding the new tail must be the previous node of old src.tail.prev */
        assert(dst.tail.prev->prev == old_src.tail.prev->prev);
        /* head of dst unchanged */
        assert(dst.head.next == old_dst.head.next);
    }

    /* 7. Unchanged fields for both lists (head.prev and tail.next are invariant) */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
