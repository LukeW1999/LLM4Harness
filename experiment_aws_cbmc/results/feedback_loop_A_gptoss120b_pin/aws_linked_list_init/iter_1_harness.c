#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound the destination and source lists */
    struct aws_linked_list *dst = malloc(sizeof(*dst));
    struct aws_linked_list *src = malloc(sizeof(*src));
    __CPROVER_assume(dst != NULL);
    __CPROVER_assume(src != NULL);

    ensure_linked_list_is_allocated(dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(dst));
    __CPROVER_assume(aws_linked_list_is_valid(src));
    __CPROVER_assume(dst != src);

    /* Save old state */
    struct aws_linked_list old_dst = *dst;
    struct aws_linked_list old_src = *src;

    struct aws_linked_list_node *old_dst_head_next = dst->head.next;
    struct aws_linked_list_node *old_dst_tail_prev = dst->tail.prev;
    struct aws_linked_list_node *old_src_head_next = src->head.next;
    struct aws_linked_list_node *old_src_tail_prev = src->tail.prev;

    /* Call function under test */
    aws_linked_list_move_all_back(dst, src);

    /* Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(dst));
    assert(aws_linked_list_is_valid(src));

    /* Post‑condition: source list must be empty */
    assert(src->head.next == &src->tail);
    assert(src->tail.prev == &src->head);

    /* If source was originally empty, destination must be unchanged */
    if (old_src_head_next == &old_src.tail) {
        assert(dst->head.next == old_dst_head_next);
        assert(dst->tail.prev == old_dst_tail_prev);
    } else {
        /* Source was non‑empty: destination now ends with the former source tail */
        assert(dst->tail.prev == old_src_tail_prev);

        /* The node that previously was the last of the original destination
         * (old_dst_tail_prev) must now point to the former first source node */
        if (old_dst_tail_prev != &old_dst.tail) {
            assert(old_dst_tail_prev->next == old_src_head_next);
        } else {
            /* Destination was empty originally, so its head now points to the former source head */
            assert(dst->head.next == old_src_head_next);
        }

        /* The former source head must have its prev pointer updated to the former
         * destination tail (or the list head if destination was empty) */
        if (old_dst_tail_prev != &old_dst.tail) {
            assert(old_src_head_next->prev == old_dst_tail_prev);
        } else {
            assert(old_src_head_next->prev == &dst->head);
        }
    }

    /* No other fields of the list structures are modified */
    assert(dst->head.prev == NULL);
    assert(dst->tail.next == NULL);
    assert(src->head.prev == NULL);
    assert(src->tail.next == NULL);

    free(dst);
    free(src);
}
