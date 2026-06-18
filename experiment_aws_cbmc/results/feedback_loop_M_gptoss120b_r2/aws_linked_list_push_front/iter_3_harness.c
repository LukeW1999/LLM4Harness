#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;

    bool old_src_empty = (src.head.next == &src.tail);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post-condition assertions */

    /* Validity must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Source list must be empty after the move */
    assert(aws_linked_list_empty(&src));

    if (old_src_empty) {
        /* When source was empty, destination must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* When source was non‑empty, destination now ends with the old source tail */
        assert(dst.tail.prev == old_src_last);
        assert(dst.tail.prev->next == &dst.tail);

        /* The node that previously was the last of dst now points to the first of old src */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);

        /* The front of the destination list (first real node) is unchanged */
        assert(dst.head.next == old_dst.head.next);
    }

    /* 5. Unchanged fields that are not part of the operation */
    /* The head node's prev pointer must remain NULL */
    assert(dst.head.prev == NULL);
    assert(src.head.prev == NULL);
    /* The tail node's next pointer must remain NULL */
    assert(dst.tail.next == NULL);
    assert(src.tail.next == NULL);
}
