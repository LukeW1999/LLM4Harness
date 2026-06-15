#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src); /* dst and src must be distinct */

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* 6. Behaviour depending on whether src was originally empty */
    if (old_src.head.next == &old_src.tail) {
        /* src was empty → dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src non‑empty → src nodes appended to dst */
        /* New tail points to the former last node of src */
        assert(dst.tail.prev == old_src_last);
        /* That node now points to the list tail */
        assert(old_src_last->next == &dst.tail);
        /* The former last node of dst now points to the first node of src */
        assert(old_dst_last->next == old_src_first);
        /* The first node of src now points back to the former last node of dst */
        assert(old_src_first->prev == old_dst_last);
    }

    /* 7. Unchanged invariant fields of the list structures */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
