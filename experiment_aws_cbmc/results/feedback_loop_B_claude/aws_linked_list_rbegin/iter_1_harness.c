#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_rbegin_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* 4. Assert postconditions */

    /* The function returns list->tail.prev */
    assert(rval == old.tail.prev);
    assert(rval == list.tail.prev);

    /* 5. Assert unchanged fields — the function only reads, never writes */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));

    /* 7. The returned pointer must be non-null (tail.prev is always valid in a valid list) */
    assert(rval != NULL);
}
