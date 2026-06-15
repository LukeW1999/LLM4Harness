#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list src;
    struct aws_linked_list dst;

    /* Allocate and bound the linked lists */
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    /* Save copies for immutability checks */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑conditions */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* Source list must be empty */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Destination list must remain a valid list */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(dst.head.next != NULL);
    assert(dst.tail.prev != NULL);

    /* If the source was originally empty, destination should be unchanged */
    if (aws_linked_list_empty(&old_src)) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* Destination now contains the former source elements */
        assert(dst.tail.prev == old_src.tail.prev);
    }
}
