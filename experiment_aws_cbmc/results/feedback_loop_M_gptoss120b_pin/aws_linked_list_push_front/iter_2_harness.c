#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list src;
    struct aws_linked_list dst;

    /* Allocate linked lists with bounded length */
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Pre‑condition: both lists are valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    /* Save old state */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *src_first = src.head.next;
    struct aws_linked_list_node *src_last  = src.tail.prev;
    struct aws_linked_list_node *dst_last  = dst.tail.prev;

    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* Invariant: head.prev and tail.next are always NULL */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    if (!src_was_empty) {
        /* Source list must be empty after move */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* Destination list now ends with the former source last node */
        assert(dst.tail.prev == src_last);
        assert(src_last->next == &dst.tail);

        if (!dst_was_empty) {
            /* Original destination tail should now link to former source first */
            assert(old_dst.tail.prev->next == src_first);
            assert(src_first->prev == old_dst.tail.prev);
        } else {
            /* Destination was empty, its head should now point to former source first */
            assert(dst.head.next == src_first);
            assert(src_first->prev == &dst.head);
        }
    } else {
        /* Source was empty: it remains unchanged */
        assert(src.head.next == old_src.head.next);
        assert(src.tail.prev == old_src.tail.prev);

        /* Destination unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    }
}
