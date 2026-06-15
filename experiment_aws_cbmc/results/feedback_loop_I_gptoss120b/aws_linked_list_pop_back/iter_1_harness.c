#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = dst.head.next;
    struct aws_linked_list_node *old_dst_last  = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;

    bool src_was_empty = aws_linked_list_empty(&src);

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition checks */

    /* Both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (!src_was_empty) {
        /* src should be empty after the move */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst should now end with the former last node of src */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);

        /* the splice should connect the old dst tail to the old src head */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    } else {
        /* src was already empty – both lists unchanged */
        assert(src.head.next == old_src.head.next);
        assert(src.tail.prev == old_src.tail.prev);

        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
    }

    /* 5. Unchanged fields (head and tail node pointers other than those
       explicitly modified above) – there are no other fields in the
       struct, so no further asserts are required. */
}
