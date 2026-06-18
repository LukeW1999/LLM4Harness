#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src); /* distinct lists */

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    bool old_dst_empty = aws_linked_list_empty(&old_dst);
    bool old_src_empty = aws_linked_list_empty(&old_src);

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;   /* may be &old_dst.tail if empty */
    struct aws_linked_list_node *old_src_first = old_src.head.next;  /* undefined if src empty */
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;  /* undefined if src empty */

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (old_src_empty) {
        /* src was empty: dst must be unchanged, src stays empty */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* src non‑empty: src becomes empty */
        assert(aws_linked_list_empty(&src));
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst now contains old dst nodes followed by old src nodes */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);

        if (old_dst_empty) {
            /* dst was empty, its first element is now the first element of old src */
            assert(dst.head.next == old_src_first);
            assert(old_src_first->prev == &dst.head);
        } else {
            /* dst had elements, link old last of dst to first of old src */
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
        }
    }
}
