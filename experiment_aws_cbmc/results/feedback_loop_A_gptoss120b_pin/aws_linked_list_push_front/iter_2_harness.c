#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Unchanged invariants for list sentinel nodes */
    assert(dst.head->prev == NULL);
    assert(dst.tail->next == NULL);
    assert(src.head->prev == NULL);
    assert(src.tail->next == NULL);

    /* Determine emptiness of original lists */
    bool src_was_empty = aws_linked_list_empty(&old_src);
    bool dst_was_empty = aws_linked_list_empty(&old_dst);

    if (src_was_empty) {
        /* No nodes moved; both lists unchanged */
        assert(dst.head->next == old_dst.head->next);
        assert(dst.tail->prev == old_dst.tail->prev);
        assert(src.head->next == old_src.head->next);
        assert(src.tail->prev == old_src.tail->prev);
    } else {
        /* src should be empty after move */
        assert(src.head->next == &src.tail);
        assert(src.tail->prev == &src.head);

        /* dst.tail->prev must be the former last node of src */
        assert(dst.tail->prev == old_src.tail->prev);

        if (dst_was_empty) {
            /* dst was empty, its first node is now the former first node of src */
            assert(dst.head->next == old_src.head->next);
        } else {
            /* dst not empty, its first node unchanged */
            assert(dst.head->next == old_dst.head->next);
            /* The original last node of dst should now point to the former first node of src */
            assert(old_dst.tail->prev->next == old_src.head->next);
            assert(old_src.head->next->prev == old_dst.tail->prev);
        }
    }
}
