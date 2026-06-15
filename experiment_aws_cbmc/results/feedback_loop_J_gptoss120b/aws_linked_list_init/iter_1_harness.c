#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness() {
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
    bool src_was_empty = aws_linked_list_empty(&src);

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition checks */

    /* validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(aws_linked_list_empty(&src));

    /* head.prev and tail.next are immutable for a well‑formed list */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        /* when src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* when src was non‑empty, dst now ends with the former src tail */
        assert(dst.tail.prev == old_src.tail.prev);

        /* the former last node of dst now points to the former first node of src */
        assert(old_dst.tail.prev->next == old_src.head.next);

        /* the former first node of src now points back to the former last node of dst */
        assert(old_src.head.next->prev == old_dst.tail.prev);
    }

    /* the two list objects remain distinct */
    assert(&dst != &src);
}
