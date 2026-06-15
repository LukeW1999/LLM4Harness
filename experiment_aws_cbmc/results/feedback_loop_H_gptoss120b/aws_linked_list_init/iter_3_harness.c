#include <assert.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate two linked lists with nondeterministic contents */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Nondeterministically make src empty or keep its allocated contents */
    if (__CPROVER_nondet_bool()) {
        src.head.next = &src.tail;
        src.tail.prev = &src.head;
    }

    /* Save old state for post‑condition checks */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Post‑condition: src is empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Post‑condition: dst reflects the splice */
    if (old_src_first == &old_src.tail) {
        /* src was empty – dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src non‑empty – new tail is the old src last node */
        assert(dst.tail.prev == old_src_last);
        /* The node before the new tail is the old dst last node */
        assert(old_src_last->prev == old_dst_last);
        /* Old dst last node now points forward to old src first node */
        assert(old_dst_last->next == old_src_first);
        /* Old src first node points back to old dst last node */
        assert(old_src_first->prev == old_dst_last);
    }

    /* Unchanged sentinel fields of both lists */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
