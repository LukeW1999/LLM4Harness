#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are initially valid */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Save pointers to important nodes in the old state */
    struct aws_linked_list_node *old_dst_back = old_dst.tail.prev;   /* may be &old_dst.tail if empty */
    struct aws_linked_list_node *old_src_front = old_src.head.next;  /* may be &old_src.tail if empty */
    struct aws_linked_list_node *old_src_back  = old_src.tail.prev;  /* may be &old_src.head if empty */

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postcondition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Postcondition: src is empty after the operation */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Distinguish whether src was empty before the call */
    if (old_src.head.next == &old_src.tail) {
        /* src was empty: dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.head.prev == NULL);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.tail.next == NULL);
    } else {
        /* src was non‑empty: nodes from src are appended to dst */

        /* The new back of dst should be the old back of src */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);

        /* The node that was previously the back of dst should now point to the first node of src */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);
    }

    /* Unchanged fields for dst that are not affected by the operation */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
