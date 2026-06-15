#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate source and destination lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Non‑deterministically decide whether each list is initially empty */
    bool src_empty = nondet_bool();
    bool dst_empty = nondet_bool();

    /* Initialise src list */
    if (src_empty) {
        src.head.next = &src.tail;
        src.head.prev = NULL;
        src.tail.prev = &src.head;
        src.tail.next = NULL;
    } else {
        struct aws_linked_list_node *src_node = malloc(sizeof(*src_node));
        __CPROVER_assume(src_node != NULL);
        src_node->prev = &src.head;
        src_node->next = &src.tail;
        src.head.next = src_node;
        src.head.prev = NULL;
        src.tail.prev = src_node;
        src.tail.next = NULL;
    }

    /* Initialise dst list */
    if (dst_empty) {
        dst.head.next = &dst.tail;
        dst.head.prev = NULL;
        dst.tail.prev = &dst.head;
        dst.tail.next = NULL;
    } else {
        struct aws_linked_list_node *dst_node = malloc(sizeof(*dst_node));
        __CPROVER_assume(dst_node != NULL);
        dst_node->prev = &dst.head;
        dst_node->next = &dst.tail;
        dst.head.next = dst_node;
        dst.head.prev = NULL;
        dst.tail.prev = dst_node;
        dst.tail.next = NULL;
    }

    /* Assume the lists satisfy the validity predicate */
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Precondition: distinct lists */
    __CPROVER_assume(&src != &dst);

    /* Save old state for post‑condition checks */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;
    struct aws_linked_list_node *old_dst_last  = dst.tail.prev;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑conditions: both lists must remain valid */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (old_src_first == &old_src.tail) {
        /* src was originally empty: dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src was non‑empty: its nodes are now appended to dst */
        assert(dst.tail.prev == old_src_last);
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
        assert(old_src_last->next == &dst.tail);
    }
}
