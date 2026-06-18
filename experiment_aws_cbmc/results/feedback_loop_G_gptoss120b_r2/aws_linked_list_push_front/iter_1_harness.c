#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

/* Helper to count data nodes (excluding head and tail) */
static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list *dst = malloc(sizeof *dst);
    struct aws_linked_list *src = malloc(sizeof *src);
    __CPROVER_assume(dst != NULL);
    __CPROVER_assume(src != NULL);
    ensure_linked_list_is_allocated(dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(dst));
    __CPROVER_assume(aws_linked_list_is_valid(src));
    __CPROVER_assume(dst != src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = *dst;
    struct aws_linked_list old_src = *src;

    struct aws_linked_list_node *old_dst_last   = dst->tail.prev;
    struct aws_linked_list_node *old_dst_first  = dst->head.next;
    struct aws_linked_list_node *old_src_first  = src->head.next;
    struct aws_linked_list_node *old_src_last   = src->tail.prev;

    size_t old_dst_count = count_nodes(dst);
    size_t old_src_count = count_nodes(src);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(dst, src);

    /* 4. Post‑condition assertions */

    /* validity invariants */
    assert(aws_linked_list_is_valid(dst));
    assert(aws_linked_list_is_valid(src));

    /* src must be empty */
    assert(src->head.next == &src->tail);
    assert(src->tail.prev == &src->head);

    /* dst must contain the original elements followed by the moved ones */
    size_t new_dst_count = count_nodes(dst);
    assert(new_dst_count == old_dst_count + old_src_count);

    if (old_src_count == 0) {
        /* src was empty → dst unchanged */
        assert(dst->tail.prev == old_dst_last);
        assert(dst->head.next == old_dst_first);
    } else {
        /* src non‑empty → tail now points to the former src last node */
        assert(dst->tail.prev == old_src_last);
        /* linkage between the former last node of dst and the first node of src */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    }

    /* 5. Unchanged fields that are not part of the operation */
    /* The allocator, if any, is not part of aws_linked_list, so no extra unchanged‑field asserts are needed. */

    /* clean up */
    free(dst);
    free(src);
}
