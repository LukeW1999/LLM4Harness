#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Declare and initialize dst and src linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    /* dst != src (AWS_RESTRICT) */
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    bool src_was_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *src_front = src.head.next;
    struct aws_linked_list_node *src_back = src.tail.prev;
    struct aws_linked_list_node *dst_back = dst.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* src must always be valid and empty after the call */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    /* dst must always be valid after the call */
    assert(aws_linked_list_is_valid(&dst));

    if (!src_was_empty) {
        /* src nodes were spliced into dst at the back */
        /* dst's new back should be src's old back */
        assert(dst.tail.prev == src_back);
        /* src's old front should follow dst's old back */
        assert(dst_back->next == src_front);
        assert(src_front->prev == dst_back);
        assert(src_back->next == &dst.tail);
    } else {
        /* src was empty, dst should be unchanged */
        assert(dst.tail.prev == dst_back);
    }

    /* src is now empty: head.next == &src.tail and tail.prev == &src.head */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* src head and tail sentinel pointers are unchanged */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* dst head and tail sentinel pointers are unchanged */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
