#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness() {
    /* 1. Declare and initialize dst and src linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions: both lists must be valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    /* dst != src is required by the function (AWS_RESTRICT) */
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state before calling */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save pointers to key nodes before the call */
    struct aws_linked_list_node *dst_old_back = dst.tail.prev;
    struct aws_linked_list_node *src_old_front = src.head.next;
    struct aws_linked_list_node *src_old_back = src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* src must be empty after the call */
    assert(aws_linked_list_empty(&src));

    /* src must be valid after the call */
    assert(aws_linked_list_is_valid(&src));

    /* dst must be valid after the call */
    assert(aws_linked_list_is_valid(&dst));

    /* src head and tail must point to each other (empty list) */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (src_was_empty) {
        /* If src was empty, dst should be unchanged */
        assert(dst.tail.prev == dst_old_back);
        assert(dst.head.next == (dst_was_empty ? &dst.tail : dst.head.next));
    } else {
        /* If src was not empty, src nodes are now at the back of dst */
        /* The last node of dst should be the old last node of src */
        assert(dst.tail.prev == src_old_back);
        /* The old last node of src should point to dst's tail */
        assert(src_old_back->next == &dst.tail);
        /* The old front of src should be connected to old back of dst */
        assert(dst_old_back->next == src_old_front);
        assert(src_old_front->prev == dst_old_back);
    }

    /* src head.prev must remain NULL (invariant of linked list) */
    assert(src.head.prev == NULL);
    /* src tail.next must remain NULL (invariant of linked list) */
    assert(src.tail.next == NULL);

    /* dst head.prev must remain NULL (invariant of linked list) */
    assert(dst.head.prev == NULL);
    /* dst tail.next must remain NULL (invariant of linked list) */
    assert(dst.tail.next == NULL);
}
