#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness() {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 2. Assume both lists are initially valid */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* 3. Assume the two lists are distinct */
    __CPROVER_assume(&dst != &src);

    /* 4. Save old state for later comparison */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* 5. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 6. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 7. Post‑condition: src must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* 8. Invariants that never change */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 9. Behaviour when src was originally empty: dst unchanged */
    if (old_src.head.next == &old_src.tail) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* 10. When src had elements, they are appended to dst */

        /* The new last node of dst is the former last node of src */
        assert(dst.tail.prev == old_src.tail.prev);
        assert(dst.tail.prev->next == &dst.tail);

        /* The node preceding the new last node is the former last node of dst */
        struct aws_linked_list_node *orig_dst_last = old_dst.tail.prev;
        assert(dst.tail.prev->prev == orig_dst_last);

        /* The former first node of src is now linked after the original last node of dst */
        assert(orig_dst_last->next == old_src.head.next);
        assert(old_src.head.next->prev == orig_dst_last);
    }
}
