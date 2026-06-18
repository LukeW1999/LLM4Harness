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
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state before calling */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save key pointers for postcondition checks */
    struct aws_linked_list_node *src_front_before = src.head.next;
    struct aws_linked_list_node *src_back_before = src.tail.prev;
    struct aws_linked_list_node *dst_back_before = dst.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* src must be valid and empty after the call */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    /* src head and tail point to each other */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* dst must be valid after the call */
    assert(aws_linked_list_is_valid(&dst));

    if (src_was_empty) {
        /* If src was empty, dst should be unchanged */
        if (dst_was_empty) {
            assert(aws_linked_list_empty(&dst));
        }
        /* dst's tail.prev should still be dst_back_before */
        assert(dst.tail.prev == dst_back_before);
    } else {
        /* If src was non-empty, src nodes are now at the back of dst */
        /* dst's last node should be what was src's last node */
        assert(dst.tail.prev == src_back_before);
        /* src's back node's next should point to dst's tail */
        assert(src_back_before->next == &dst.tail);
        /* src's front node should be connected to dst's old back */
        assert(src_front_before->prev == dst_back_before);
        /* dst's old back should point to src's front */
        assert(dst_back_before->next == src_front_before);
        /* dst is not empty */
        assert(!aws_linked_list_empty(&dst));
    }

    /* Validity invariants */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));
}
