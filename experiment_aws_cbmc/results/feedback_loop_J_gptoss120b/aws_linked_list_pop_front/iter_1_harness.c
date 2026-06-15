#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition: src must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    /* head and tail invariants for src */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 5. Post‑condition: dst must remain a valid list */
    assert(aws_linked_list_is_valid(&dst));
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    /* 6. Preserve ordering semantics */
    if (old_src.head.next == &old_src.tail) {
        /* src was empty: dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src non‑empty: dst now ends with the former src nodes */
        assert(dst.tail.prev == old_src.tail.prev);

        if (old_dst.head.next == &old_dst.tail) {
            /* dst was empty: its first node is the former src front */
            assert(dst.head.next == old_src.head.next);
            assert(old_src.head.next->prev == &dst.head);
        } else {
            /* dst non‑empty: its first node unchanged */
            assert(dst.head.next == old_dst.head.next);
            /* linkage between old dst tail and old src front */
            assert(old_dst.tail.prev->next == old_src.head.next);
            assert(old_src.head.next->prev == old_dst.tail.prev);
        }
    }

    /* 7. Unchanged fields that are not part of the move operation */
    /* The head.prev and tail.next of both lists remain NULL (already asserted) */
    /* No other fields exist in struct aws_linked_list, so nothing else to assert */
}
