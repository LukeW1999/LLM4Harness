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

    /* 2. Preconditions: both lists must be valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    /* dst != src is required by the function (AWS_RESTRICT) */
    __CPROVER_assume(&dst != &src);

    /* 3. Save old state before calling */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save key pointers for postcondition checks */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 4. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 5. Assert postconditions */

    /* src must always be valid and empty after the call */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    /* dst must always be valid after the call */
    assert(aws_linked_list_is_valid(&dst));

    if (!src_was_empty) {
        /* src nodes were spliced into dst at the back */
        /* The old dst back should now point to old src front */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* The old src back should now point to dst tail */
        assert(old_src_back->next == &dst.tail);
        assert(dst.tail.prev == old_src_back);

        /* dst is not empty */
        assert(!aws_linked_list_empty(&dst));
    } else {
        /* src was empty, dst should be unchanged */
        assert(dst.tail.prev == old_dst_back);
        assert(old_dst_back->next == &dst.tail);
    }

    /* src head and tail sentinel pointers reset correctly */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
