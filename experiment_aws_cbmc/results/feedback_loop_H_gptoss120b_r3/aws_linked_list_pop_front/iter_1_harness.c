#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness() {
    /* Allocate and initialize destination list */
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&dst);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Allocate and initialize source list */
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&src);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Non‑deterministically decide lengths (bounded) */
    size_t dst_len = nondet_uint8_t() % (MAX_LINKED_LIST_ITEM_ALLOCATION + 1);
    size_t src_len = nondet_uint8_t() % (MAX_LINKED_LIST_ITEM_ALLOCATION + 1);

    /* Build destination list */
    for (size_t i = 0; i < dst_len; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&dst, node);
        __CPROVER_assume(aws_linked_list_is_valid(&dst));
    }

    /* Build source list */
    for (size_t i = 0; i < src_len; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&src, node);
        __CPROVER_assume(aws_linked_list_is_valid(&src));
    }

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = old_dst.head.next;
    struct aws_linked_list_node *old_dst_last  = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity invariants must hold */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Source list must be empty after the move */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Destination list post‑conditions */
    if (!aws_linked_list_empty(&old_src)) {
        /* src was non‑empty, its nodes are now at the back of dst */
        assert(dst.tail.prev == old_src_last);
        if (!aws_linked_list_empty(&old_dst)) {
            /* there were existing nodes in dst */
            assert(old_dst_last->next == old_src_first);
            assert(old_src_first->prev == old_dst_last);
        } else {
            /* dst was empty, src becomes the whole dst */
            assert(dst.head.next == old_src_first);
            assert(old_src_first->prev == &dst.head);
        }
    } else {
        /* src was empty, dst must be unchanged */
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
        if (!aws_linked_list_empty(&dst)) {
            assert(old_dst_first->prev == &dst.head);
            assert(old_dst_last->next == &dst.tail);
        } else {
            assert(dst.head.next == &dst.tail);
            assert(dst.tail.prev == &dst.head);
        }
    }
}
