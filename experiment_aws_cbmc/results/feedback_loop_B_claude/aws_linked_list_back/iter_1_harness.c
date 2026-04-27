#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_back_harness(void) {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* The function requires the list to be non-empty (back() is only valid on non-empty lists) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 4. Assert postconditions */

    /* The function returns list->tail.prev */
    assert(result == list.tail.prev);

    /* The result should be the node just before the tail (i.e., the last real node) */
    assert(result != NULL);

    /* The result should not be the head sentinel (list is non-empty) */
    assert(result != &list.head);

    /* 5. Assert unchanged fields — the function only reads, never writes */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));

    /* The list should still be non-empty after the call */
    assert(!aws_linked_list_empty(&list));
}
