#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound the destination list */
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Allocate and bound the source list */
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Ensure the two lists are distinct */
    __CPROVER_assume(&dst != &src);

    /* Save old state for later comparison */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postcondition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Postcondition: source list must be empty after the operation */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Unchanged invariants for both lists */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Determine whether source was empty before the call */
    if (old_src.head.next == &old_src.tail) {
        /* Source was empty: destination list must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == old_dst.head.prev);
        assert(dst.tail.next == old_dst.tail.next);
    } else {
        /* Source was non‑empty: destination list is extended with source nodes */

        /* The new last node of dst should be the old last node of src */
        assert(dst.tail.prev == old_src.tail.prev);
        /* That node must point back to the new tail */
        assert(old_src.tail.prev->next == &dst.tail);
        /* The old last node of dst should now point forward to the old first node of src */
        assert(old_dst.tail.prev->next == old_src.head.next);
        /* The old first node of src should point back to the old last node of dst */
        assert(old_src.head.next->prev == old_dst.tail.prev);
    }
}
