#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_move_all_back_harness() {
    /* Non-deterministic lists */
    struct aws_linked_list dst, src;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state for postcondition assertions */
    struct aws_linked_list_node *old_dst_front = dst.head.next;
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;
    bool old_dst_empty = aws_linked_list_empty(&dst);
    bool old_src_empty = aws_linked_list_empty(&src);

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postconditions */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    /* If src was non-empty, the last node of dst is the old last node of src */
    if (!old_src_empty) {
        assert(dst.tail.prev == old_src_back);
        /* The first node of dst depends on whether dst was empty */
        if (old_dst_empty) {
            assert(dst.head.next == old_src_front);
        } else {
            assert(dst.head.next == old_dst_front);
        }
    } else {
        /* If src was empty, dst is unchanged */
        assert(dst.tail.prev == old_dst_back);
        assert(dst.head.next == old_dst_front);
    }

    /* Sentinels remain valid: head.prev == NULL and tail.next == NULL */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
