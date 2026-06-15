#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

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

    /* 3. Optionally populate src with 1 or 2 nodes */
    bool src_has_nodes = nondet_bool();
    struct aws_linked_list_node *src_front = NULL;
    struct aws_linked_list_node *src_back  = NULL;

    if (src_has_nodes) {
        struct aws_linked_list_node *n1 = malloc(sizeof(*n1));
        __CPROVER_assume(n1 != NULL);
        aws_linked_list_push_back(&src, n1);

        bool add_second = nondet_bool();
        if (add_second) {
            struct aws_linked_list_node *n2 = malloc(sizeof(*n2));
            __CPROVER_assume(n2 != NULL);
            aws_linked_list_push_back(&src, n2);
        }

        __CPROVER_assume(aws_linked_list_is_valid(&src));

        src_front = src.head.next;
        src_back  = src.tail.prev;
    }

    /* Capture the original last node of dst (may be the tail sentinel) */
    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 5. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 6. Unchanged fields: the list structures themselves (head/tail objects) */
    assert(&dst.head == &old_dst.head);
    assert(&dst.tail == &old_dst.tail);
    assert(&src.head == &old_src.head);
    assert(&src.tail == &old_src.tail);

    if (!src_has_nodes) {
        /* src was empty: dst must be unchanged and src stays empty */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* src was non‑empty: src must be empty after the move */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst now ends with the former src_back node */
        assert(dst.tail.prev == src_back);

        /* The first node transferred (src_front) must be linked after the
           original last node of dst (or directly after head if dst was empty) */
        if (old_dst_last == &old_dst.tail) {
            /* dst was empty before the move */
            assert(dst.head.next == src_front);
        } else {
            assert(src_front->prev == old_dst_last);
            assert(old_dst_last->next == src_front);
        }
    }
}
