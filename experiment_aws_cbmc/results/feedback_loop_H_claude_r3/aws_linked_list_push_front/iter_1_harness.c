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

    /* 2. Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    /* dst != src (AWS_RESTRICT precondition) */
    __CPROVER_assume(&dst != &src);

    /* 3. Save old state */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save key pointers before the call */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 4. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 5. Assert postconditions */

    /* src must be empty after the call */
    assert(aws_linked_list_empty(&src));

    /* src must be valid */
    assert(aws_linked_list_is_valid(&src));

    /* dst must be valid */
    assert(aws_linked_list_is_valid(&dst));

    if (src_was_empty) {
        /* If src was empty, dst should be unchanged */
        /* dst's back is still the same node */
        assert(dst.tail.prev == old_dst_back);
    } else {
        /* If src was not empty, the nodes from src are now at the back of dst */
        /* The old dst back should now point to old src front */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* dst's tail should now point to old src back */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);
    }

    /* src head and tail should be reset to point to each other */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* src head.prev must remain NULL (invariant of linked list) */
    assert(src.head.prev == NULL);
    /* src tail.next must remain NULL (invariant of linked list) */
    assert(src.tail.next == NULL);

    /* dst head.prev must remain NULL */
    assert(dst.head.prev == NULL);
    /* dst tail.next must remain NULL */
    assert(dst.tail.next == NULL);
}
