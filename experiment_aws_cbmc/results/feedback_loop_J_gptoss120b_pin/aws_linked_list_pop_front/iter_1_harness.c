#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

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

    struct aws_linked_list_node *old_dst_back = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_front = old_src.head.next;
    struct aws_linked_list_node *old_src_back = old_src.tail.prev;

    bool src_was_empty = (old_src.head.next == &old_src.tail);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Postconditions */

    /* Validity must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (src_was_empty) {
        /* When src is empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);
    } else {
        /* src was non‑empty: src becomes empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
        assert(src.head.prev == NULL);
        assert(src.tail.next == NULL);

        /* dst now ends with the former src list */
        assert(dst.tail.prev == old_src_back);
        assert(dst.tail.prev->next == &dst.tail);
        assert(old_src_back->prev == old_src_back->prev); /* unchanged link before back */

        /* linking points are correct */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);
    }

    /* 5. Unchanged fields for both lists (head.prev, tail.next never change) */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
