#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Behaviour depending on whether src was empty */
    if (old_src.head.next == &old_src.tail) {
        /* src was empty: both lists must be unchanged */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);
    } else {
        /* src non‑empty: src becomes empty, dst gets the nodes */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst connections */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* head of dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.head.prev == NULL);
        assert(dst.tail.next == NULL);
    }
}
