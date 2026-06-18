#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* The harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness() {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);               /* dst and src must be distinct */

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;   /* may be &old_dst.head if empty */
    struct aws_linked_list_node *old_src_first = old_src.head.next;  /* may be &old_src.tail if empty */
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;   /* may be &old_src.head if empty */

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 5. src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* 6. Behaviour depends on whether src was originally empty */
    if (old_src_first == &old_src.tail) {
        /* src was empty: dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src was non‑empty: dst now ends with the former src elements */
        /* The first element of dst is unchanged */
        assert(dst.head.next == old_dst.head.next);

        /* The last element of dst is the former last element of src */
        assert(dst.tail.prev == old_src_last);

        /* The node that used to be the last of dst now points to the first of src */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);

        /* The former last of src now points to the tail sentinel of dst */
        assert(old_src_last->next == &dst.tail);
        assert(dst.tail.prev->next == &dst.tail);   /* tail's next is always NULL, checked via validity */
    }

    /* 7. No other fields of the list structures are modified (head and tail nodes themselves
       are part of the list structure, so their next/prev pointers are allowed to change as
       required by the operation). All node objects themselves remain the same objects. */
}
