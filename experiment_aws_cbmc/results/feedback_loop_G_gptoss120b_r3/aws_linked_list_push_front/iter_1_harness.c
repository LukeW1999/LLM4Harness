#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are initially valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postcondition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Postcondition: src list is empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Unchanged fields of src (apart from head/ tail linkage which are set to empty) */
    /* head.prev and tail.next are always NULL for a valid list */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Unchanged fields of dst */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(dst.head.next == old_dst.head.next); /* first element unchanged */

    if (old_src.head.next == &old_src.tail) {
        /* src was empty: dst should be unchanged */
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.tail.prev->next == &dst.tail);
    } else {
        /* src was non‑empty: dst.tail.prev should now be the former src.tail.prev */
        assert(dst.tail.prev == old_src.tail.prev);
        assert(dst.tail.prev->next == &dst.tail);
        /* The node that was previously the last of dst should now link to the former first of src */
        assert(old_dst.tail.prev->next == old_src.head.next);
        assert(old_src.head.next->prev == old_dst.tail.prev);
    }
}
