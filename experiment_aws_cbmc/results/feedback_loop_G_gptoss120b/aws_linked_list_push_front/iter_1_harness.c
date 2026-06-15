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
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_back = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_front = old_src.head.next;
    struct aws_linked_list_node *old_src_back  = old_src.tail.prev;

    bool old_dst_empty = aws_linked_list_empty(&old_dst);
    bool old_src_empty = aws_linked_list_empty(&old_src);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Post‑condition: src is empty after the move */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 6. Post‑condition: unchanged fields of the list structures */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 7. Post‑condition: behavior depending on whether src was empty */
    if (old_src_empty) {
        /* src empty → dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src non‑empty → dst now ends with the former src elements */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);
    }
}
