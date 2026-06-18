#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>
#include <assert.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑conditions */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty */
    assert(src.head == NULL);
    assert(src.tail == NULL);

    if (old_src.head == NULL) {
        /* src was empty, dst unchanged */
        assert(dst.head == old_dst.head);
        assert(dst.tail == old_dst.tail);
    } else {
        /* src contributed nodes */
        if (old_dst.head == NULL) {
            /* dst was empty, now equals old src */
            assert(dst.head == old_src.head);
        } else {
            /* dst non‑empty, head unchanged */
            assert(dst.head == old_dst.head);
            /* linkage between old dst tail and old src head */
            struct aws_linked_list_node *old_dst_tail = old_dst.tail;
            struct aws_linked_list_node *old_src_head = old_src.head;
            assert(old_dst_tail->next == old_src_head);
            assert(old_src_head->prev == old_dst_tail);
        }
        /* tail must be old src tail */
        assert(dst.tail == old_src.tail);
    }
}
