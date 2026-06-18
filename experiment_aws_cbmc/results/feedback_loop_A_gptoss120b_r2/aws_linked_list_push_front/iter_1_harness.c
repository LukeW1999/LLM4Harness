#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src); /* distinct objects */

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Post‑condition: src is empty */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* 6. Post‑condition: dst contains the concatenation of the old lists */
    bool src_was_empty = aws_linked_list_empty(&old_src);
    bool dst_was_empty = aws_linked_list_empty(&old_dst);

    if (src_was_empty) {
        /* src empty → dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        if (dst_was_empty) {
            /* dst empty → dst becomes old src */
            assert(dst.head.next == old_src.head.next);
            assert(dst.tail.prev == old_src.tail.prev);
        } else {
            /* both non‑empty → splice */
            /* old last node of dst now points to first node of src */
            assert(old_dst.tail.prev->next == old_src.head.next);
            /* old last node of src now points to dst's tail sentinel */
            assert(old_src.tail.prev->next == &dst.tail);
            /* dst's tail now points to old last node of src */
            assert(dst.tail.prev == old_src.tail.prev);
            /* first element of dst unchanged */
            assert(dst.head.next == old_dst.head.next);
        }
    }

    /* 7. Invariant: head.prev is NULL and tail.next is NULL for both lists */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
