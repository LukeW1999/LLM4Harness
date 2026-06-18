#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are initially valid */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Save old copies for later comparison */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Post‑condition: src must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Determine whether src was empty before the call */
    bool src_was_empty = (old_src.head.next == &old_src.tail);

    if (src_was_empty) {
        /* When src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* When src was non‑empty, dst's tail should now point to the former last node of src */
        assert(dst.tail.prev == old_src.tail.prev);
        /* The first node of src should now follow the former last node of dst */
        assert(old_dst.tail.prev->next == old_src.head.next);
        /* The new first node of src should have its prev set to the former last node of dst */
        assert(old_src.head.next->prev == old_dst.tail.prev);
    }

    /* Unchanged fields for dst that are not affected by the operation */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
