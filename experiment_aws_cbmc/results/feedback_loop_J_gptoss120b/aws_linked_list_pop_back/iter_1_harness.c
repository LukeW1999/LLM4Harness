#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* The maximum number of nodes that can be allocated in a list for the proof.
 * This macro is defined in the Makefile used by the verification harnesses. */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 4
#endif

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src); /* distinct objects */

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = dst.head.next;
    struct aws_linked_list_node *old_dst_last  = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. Post‑condition: src must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* 6. Post‑condition: dst reflects the splice */
    if (old_src_first == &old_src.tail) {
        /* src was empty – dst must be unchanged */
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
    } else {
        /* src was non‑empty – its nodes are appended to dst */
        /* The new tail of dst is the former last node of src */
        assert(dst.tail.prev == old_src_last);
        /* The node that previously was the last of dst now points to the first of src */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
        /* The head of dst is unchanged */
        assert(dst.head.next == old_dst_first);
    }

    /* 7. Unchanged fields of the list structures (head and tail node contents) */
    /* The head node's prev is always NULL and the tail node's next is always NULL */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
