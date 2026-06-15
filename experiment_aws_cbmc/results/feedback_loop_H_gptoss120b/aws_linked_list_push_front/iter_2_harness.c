#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    /* allocate and bound the lists */
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* nondeterministically decide whether src is initially empty */
    bool src_was_empty = __CPROVER_nondet_bool();

    if (src_was_empty) {
        /* make src empty */
        src.head.next = &src.tail;
        src.tail.prev = &src.head;
        src.head.prev = NULL;
        src.tail.next = NULL;
    } else {
        /* ensure src has at least one node */
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&src, node);
    }

    /* save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;
    struct aws_linked_list_node *old_dst_last = dst.tail.prev;
    struct aws_linked_list_node *old_src_last = src.tail.prev; /* may be &src.head if src empty */

    /* call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* post‑conditions */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        /* dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);
    } else {
        /* dst now ends with the former src last node */
        assert(dst.tail.prev == old_src_last);
        /* the node that previously was the last of dst now links to old_src_last */
        assert(old_dst_last->next == old_src_last);
        assert(old_src_last->prev == old_dst_last);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);
    }
}
