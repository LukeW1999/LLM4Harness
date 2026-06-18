#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound the destination and source linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Ensure the two lists are distinct objects */
    __CPROVER_assume(&dst != &src);

    /* Snapshot old state for later comparison */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Record whether the source list was empty before the operation */
    bool src_was_empty = aws_linked_list_empty(&src);

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Post‑condition: source list must be empty after the operation */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Post‑condition: the head of the destination list is unchanged */
    assert(dst.head.next == old_dst.head.next);

    if (src_was_empty) {
        /* When source was empty, destination list is unchanged */
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* When source was non‑empty, destination tail now points to the former source tail */
        assert(dst.tail.prev == old_src.tail.prev);

        /* Verify the splicing connections */
        assert(old_dst.tail.prev->next == old_src.head.next);
        assert(old_src.head.next->prev == old_dst.tail.prev);
        assert(old_src.tail.prev->next == &dst.tail);
    }
}
