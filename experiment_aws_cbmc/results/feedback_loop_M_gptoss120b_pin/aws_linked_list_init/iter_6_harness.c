#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate allocator and bound the two lists */
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

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

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition assertions */

    /* validity invariants */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(aws_linked_list_empty(&src));

    /* head of dst unchanged (splicing at the back) */
    assert(dst.head.next == old_dst_first);

    if (src_was_empty) {
        /* when src was empty, dst unchanged */
        assert(dst.tail.prev == old_dst_last);
    } else {
        /* when src had elements, dst's new last node is the old src last node */
        assert(dst.tail.prev == old_src_last);
        /* the node that was previously the last of dst now points to the first of src */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    }

    /* unchanged fields of dst and src structures (head/tail pointers that are not modified) */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
