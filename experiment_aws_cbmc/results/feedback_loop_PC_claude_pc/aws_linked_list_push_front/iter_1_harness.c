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
    /* dst and src must be different (AWS_RESTRICT) */
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state before the call */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save pointers to key nodes before the call */
    struct aws_linked_list_node *dst_old_back = dst.tail.prev;
    struct aws_linked_list_node *src_old_front = src.head.next;
    struct aws_linked_list_node *src_old_back = src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* Postcondition: src is always valid after the call */
    assert(aws_linked_list_is_valid(&src));

    /* Postcondition: dst is always valid after the call */
    assert(aws_linked_list_is_valid(&dst));

    /* Postcondition: src is always empty after the call */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (src_was_empty) {
        /* If src was empty, dst should be unchanged */
        /* dst's back is still the same node */
        assert(dst.tail.prev == dst_old_back);
        /* dst's emptiness is preserved */
        assert(aws_linked_list_empty(&dst) == dst_was_empty);
    } else {
        /* If src was not empty, src nodes are now at the back of dst */
        /* The old back of dst now points to the old front of src */
        assert(dst_old_back->next == src_old_front);
        assert(src_old_front->prev == dst_old_back);

        /* The old back of src now points to dst's tail */
        assert(dst.tail.prev == src_old_back);
        assert(src_old_back->next == &dst.tail);

        /* dst is not empty */
        assert(!aws_linked_list_empty(&dst));
    }

    /* Postcondition: src head and tail internal pointers are valid */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Postcondition: dst head and tail internal pointers are valid */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
