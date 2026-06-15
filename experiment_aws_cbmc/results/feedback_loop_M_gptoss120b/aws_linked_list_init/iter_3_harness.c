#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

/* Helper to count nodes in a list; bounded by MAX_LINKED_LIST_ITEM_ALLOCATION */
static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail && cnt < MAX_LINKED_LIST_ITEM_ALLOCATION) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* dst and src must be distinct objects */
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    size_t old_dst_count = count_nodes(&dst);
    size_t old_src_count = count_nodes(&src);

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition assertions */

    /* Both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* dst must contain the original dst nodes followed by the original src nodes */
    {
        size_t new_dst_count = count_nodes(&dst);
        assert(new_dst_count == old_dst_count + old_src_count);
    }

    /* Unchanged fields: head.prev and tail.next stay NULL for both lists */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Preserve the first node of the original dst (if any) */
    if (old_dst_count > 0) {
        assert(dst.head.next == old_dst.head.next);
    } else {
        /* If dst was empty, the first node should be the former first node of src (if any) */
        if (old_src_count > 0) {
            assert(dst.head.next == old_src.head.next);
        } else {
            /* Both were empty, dst remains empty */
            assert(dst.head.next == &dst.tail);
        }
    }

    /* Preserve the last node of the original src (if any) as the new tail.prev */
    if (old_src_count > 0) {
        assert(dst.tail.prev == old_src.tail.prev);
    } else {
        assert(dst.tail.prev == old_dst.tail.prev);
    }
}
