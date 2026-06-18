#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Bounding constant defined in the Makefile */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate and bound source and destination lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    aws_linked_list_init(&src);
    aws_linked_list_init(&dst);

    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Save old state (pointers to first/last nodes) */
    struct aws_linked_list_node *src_old_first = src.head;
    struct aws_linked_list_node *src_old_last  = src.tail;
    struct aws_linked_list_node *dst_old_first = dst.head;
    struct aws_linked_list_node *dst_old_last  = dst.tail;

    bool src_was_empty = (src.head == NULL);
    bool dst_was_empty = (dst.head == NULL);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postconditions */

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* After move, source list must be empty */
    assert(aws_linked_list_empty(&src));

    if (src_was_empty) {
        /* If source was empty, destination must be unchanged */
        assert(dst.head == dst_old_first);
        assert(dst.tail == dst_old_last);
    } else {
        /* Source was non‑empty: destination's new last node is the old source last node */
        assert(dst.tail == src_old_last);

        /* The node that was previously the last in destination (or NULL if empty) now points to the first source node */
        if (dst_was_empty) {
            assert(dst.head == src_old_first);
        } else {
            assert(dst_old_last->next == src_old_first);
        }

        /* The first source node now points back to the previous last node of destination (or NULL if empty) */
        if (dst_was_empty) {
            assert(src_old_first->prev == NULL);
        } else {
            assert(src_old_first->prev == dst_old_last);
        }

        /* The new tail of destination must have next == NULL */
        assert(dst.tail->next == NULL);
    }
}
