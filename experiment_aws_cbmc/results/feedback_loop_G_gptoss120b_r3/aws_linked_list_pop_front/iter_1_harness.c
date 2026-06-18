#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume they are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Unchanged invariants for list heads/tails */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (!aws_linked_list_empty(&old_src)) {
        /* src should be empty after move */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst should now end with the former src elements */
        assert(dst.tail.prev == old_src.tail.prev);
        assert(dst.tail.prev->next == &dst.tail);
        assert(old_dst.tail.prev->next == old_src.head.next);
        assert(old_src.head.next->prev == old_dst.tail.prev);
    } else {
        /* src was already empty, dst unchanged */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    }
}
