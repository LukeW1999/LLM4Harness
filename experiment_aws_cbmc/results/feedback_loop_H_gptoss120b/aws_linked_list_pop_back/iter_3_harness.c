#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    /* initialize empty lists */
    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    /* nondeterministically decide sizes (bounded) */
    size_t src_len = nondet_uint();
    size_t dst_len = nondet_uint();
    __CPROVER_assume(src_len <= MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(dst_len <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* populate src list */
    for (size_t i = 0; i < src_len; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&src, node);
    }

    /* populate dst list */
    for (size_t i = 0; i < dst_len; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&dst, node);
    }

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src); /* distinct objects */

    /* save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
    struct aws_linked_list_node *old_src_first = old_src.head.next;
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;
    bool src_was_empty = (old_src.head.next == &old_src.tail);

    /* call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* post‑condition: validity invariants */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* behavior when src was empty */
    if (src_was_empty) {
        /* both lists unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(src.head.next == old_src.head.next);
        assert(src.tail.prev == old_src.tail.prev);
    } else {
        /* src becomes empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst now ends with the former src nodes */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    }

    /* unchanged fields for both lists */
    assert(dst.head.prev == NULL);
    assert(src.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.tail.next == NULL);
}
