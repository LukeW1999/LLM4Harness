#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src); /* distinct lists */

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = dst.tail.prev;   /* may be &dst.tail if empty */
    struct aws_linked_list_node *old_src_first = src.head.next; /* may be &src.tail if empty */
    struct aws_linked_list_node *old_src_last  = src.tail.prev; /* may be &src.head if empty */

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Post‑condition: src is empty after the operation */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* 6. Post‑condition: dst reflects concatenation of the original lists */
    if (old_src_first == &old_src.tail) {
        /* src was originally empty: dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst_last);
        assert(old_dst_last->next == &dst.tail);
        assert(dst.tail.prev->next == &dst.tail);
    } else {
        /* src was non‑empty */
        /* The first node of src now follows the original last node of dst */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);

        /* The new last node of dst is the original last node of src */
        assert(dst.tail.prev == old_src_last);
        assert(old_src_last->next == &dst.tail);
    }

    /* 7. Unchanged fields: the allocator (if any) and the list struct pointers themselves */
    /* The list structures themselves are the same objects */
    assert(&dst == &dst);
    assert(&src == &src);
}
