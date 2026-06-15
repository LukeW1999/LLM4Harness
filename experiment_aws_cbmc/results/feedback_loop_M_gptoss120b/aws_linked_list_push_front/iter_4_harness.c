#include <stddef.h>
#include <stdbool.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Helper to count nodes in a list (excluding head and tail) */
static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list src;
    struct aws_linked_list dst;
    struct aws_allocator *allocator = aws_default_allocator();

    const size_t max_items = 5;

    ensure_linked_list_is_allocated(&src, max_items, allocator);
    ensure_linked_list_is_allocated(&dst, max_items, allocator);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    struct aws_linked_list src_old = src;
    struct aws_linked_list dst_old = dst;

    size_t src_old_cnt = count_nodes(&src);
    size_t dst_old_cnt = count_nodes(&dst);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* Source list must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    {
        size_t src_new_cnt = count_nodes(&src);
        size_t dst_new_cnt = count_nodes(&dst);
        assert(src_new_cnt == 0);
        assert(src_new_cnt + dst_new_cnt == src_old_cnt + dst_old_cnt);
    }

    /* Invariant fields for source list */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Destination list unchanged when source was empty */
    if (src_old_cnt == 0) {
        assert(dst.head.next == dst_old.head.next);
        assert(dst.tail.prev == dst_old.tail.prev);
    }

    /* Destination list invariant fields */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
