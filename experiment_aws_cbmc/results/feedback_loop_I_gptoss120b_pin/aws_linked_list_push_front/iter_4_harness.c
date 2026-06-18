#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

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

    struct aws_linked_list_node *old_dst_head_next = old_dst.head.next;
    struct aws_linked_list_node *old_dst_tail_prev = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_head_next = old_src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = old_src.tail.prev;

    bool src_was_empty = (old_src_head_next == &old_src.tail);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (src_was_empty) {
        /* src was empty: dst must be unchanged */
        assert(dst.head.next == old_dst_head_next);
        assert(dst.tail.prev == old_dst_tail_prev);
    } else {
        /* src was non‑empty: dst now ends with the former src tail */
        assert(dst.tail.prev == old_src_tail_prev);
        /* the node that used to be the last of dst now points to the first of src */
        assert(old_dst_tail_prev->next == old_src_head_next);
        assert(old_src_head_next->prev == old_dst_tail_prev);
        /* the first element of dst is unchanged */
        assert(dst.head.next == old_dst_head_next);
    }
}
