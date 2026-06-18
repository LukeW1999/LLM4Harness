#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

struct aws_allocator *allocator = aws_default_allocator();

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list_node *old_dst_head = dst.head;
    struct aws_linked_list_node *old_dst_tail = dst.tail;
    struct aws_linked_list_node *old_src_head = src.head;
    struct aws_linked_list_node *old_src_tail = src.tail;

    bool src_was_empty = (old_src_head == NULL);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head == NULL);
    assert(src.tail == NULL);

    if (src_was_empty) {
        /* src was empty: dst must be unchanged */
        assert(dst.head == old_dst_head);
        assert(dst.tail == old_dst_tail);
    } else {
        /* src was non‑empty: dst now ends with the former src tail */
        assert(dst.tail == old_src_tail);

        if (old_dst_tail != NULL) {
            /* link old dst tail to old src head */
            assert(old_dst_tail->next == old_src_head);
            assert(old_src_head->prev == old_dst_tail);
        } else {
            /* dst was empty, now starts with src's former head */
            assert(dst.head == old_src_head);
        }

        /* the first element of dst is unchanged if it existed */
        if (old_dst_head != NULL) {
            assert(dst.head == old_dst_head);
        }
    }
}
