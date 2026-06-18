#include <aws/common/linked_list.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Nondeterministically add a few nodes to each list */
    if (nondet_bool()) {
        struct aws_linked_list_node *n = aws_mem_acquire(allocator, sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_push_back(&dst, n);
    }
    if (nondet_bool()) {
        struct aws_linked_list_node *n = aws_mem_acquire(allocator, sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_push_back(&dst, n);
    }
    if (nondet_bool()) {
        struct aws_linked_list_node *n = aws_mem_acquire(allocator, sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_push_back(&src, n);
    }
    if (nondet_bool()) {
        struct aws_linked_list_node *n = aws_mem_acquire(allocator, sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_push_back(&src, n);
    }

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_tail_prev = dst.tail.prev;
    struct aws_linked_list_node *old_src_head_next = src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = src.tail.prev;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑conditions: validity must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Unchanged invariants for list sentinel nodes */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* If src was empty before, dst must be unchanged */
    if (old_src_head_next == &src.tail) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst_tail_prev);
        if (old_dst_tail_prev != &dst.head) {
            assert(old_dst_tail_prev->next == &dst.tail);
        }
    } else {
        /* src was non‑empty: its nodes are now appended to dst */
        assert(old_src_head_next->prev == old_dst_tail_prev);
        if (old_dst_tail_prev != &dst.head) {
            assert(old_dst_tail_prev->next == old_src_head_next);
        }
        assert(dst.tail.prev == old_src_tail_prev);
        assert(old_src_tail_prev->next == &dst.tail);
    }
}
