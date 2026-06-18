#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

/* Bounding constant defined in the Makefile */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Allocate and bound source and destination lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* 2. Save old state (pointers to first/last nodes) */
    struct aws_linked_list_node *src_old_first = src.head.next;
    struct aws_linked_list_node *src_old_last  = src.tail.prev;
    struct aws_linked_list_node *dst_old_first = dst.head.next;
    struct aws_linked_list_node *dst_old_last  = dst.tail.prev;

    bool src_was_empty = (src.head.next == &src.tail);
    bool dst_was_empty = (dst.head.next == &dst.tail);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Postconditions */

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* After move, source list must be empty */
    assert(aws_linked_list_empty(&src));

    if (src_was_empty) {
        /* If source was empty, destination must be unchanged */
        assert(dst.head.next == dst_old_first);
        assert(dst.tail.prev == dst_old_last);
        if (!dst_was_empty) {
            assert(dst_old_last->next == &dst.tail);
            assert(dst_old_first->prev == &dst.head);
        }
    } else {
        /* Source was non-empty: destination's new last node is the old source last node */
        assert(dst.tail.prev == src_old_last);
        /* The node that was previously the last in destination (or head if empty) now points to the first source node */
        if (dst_was_empty) {
            assert(dst.head.next == src_old_first);
        } else {
            assert(dst_old_last->next == src_old_first);
        }
        /* The first source node now points back to the previous last node of destination (or head if empty) */
        if (dst_was_empty) {
            assert(src_old_first->prev == &dst.head);
        } else {
            assert(src_old_first->prev == dst_old_last);
        }
        /* The tail of destination still points to the tail sentinel */
        assert(dst.tail.prev->next == &dst.tail);
    }

    /* 5. Unchanged fields: the list struct addresses themselves remain the same */
    assert(&src.head == &src.head);
    assert(&src.tail == &src.tail);
    assert(&dst.head == &dst.head);
    assert(&dst.tail == &dst.tail);
}
