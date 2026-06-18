#include <aws/common/allocator.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

/* Bounding constant defined in the proof environment */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Allocate and bound the two lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);               /* distinct objects */

    /* 2. Save old state for both lists */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    bool src_was_empty = (old_src.head.next == &old_src.tail);
    bool dst_was_empty = (old_dst.head.next == &old_dst.tail);

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* 5. src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 6. dst structural changes depend on whether src was empty */
    if (src_was_empty) {
        /* nothing moved – dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* dst’s tail should now point to the former last node of src */
        assert(dst.tail.prev == old_src.tail.prev);
        assert(old_src.tail.prev->next == &dst.tail);

        /* the first node of src should be linked after the former last node of dst
         * (or directly after dst’s head if dst was empty) */
        if (dst_was_empty) {
            assert(dst.head.next == old_src.head.next);
            assert(old_src.head.next->prev == &dst.head);
        } else {
            assert(dst.head.next == old_dst.head.next);
            assert(old_dst.tail.prev->next == old_src.head.next);
            assert(old_src.head.next->prev == old_dst.tail.prev);
        }
    }

    /* 7. Invariant fields that never change */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
