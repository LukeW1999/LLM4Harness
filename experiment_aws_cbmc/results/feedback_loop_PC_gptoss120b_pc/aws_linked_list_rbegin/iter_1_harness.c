#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_rbegin_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Snapshot old state (the function must not modify the list) */
    struct aws_linked_list old = list;

    /* 3. Call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* 4. Post‑condition: return value */
    assert(rval != NULL);                                   /* tail.prev is never NULL */

    if (aws_linked_list_empty(&list)) {
        /* For an empty list rbegin should point to the head sentinel */
        assert(rval == &list.head);
    } else {
        /* For a non‑empty list rbegin should be the last real node */
        assert(rval->next == &list.tail);                   /* last node's next is tail */
        assert(rval->prev->next == rval);                   /* bidirectional link back to rbegin */
        /* The previous node may be the head sentinel (single‑element list) */
        /* No further constraints are required beyond the bidirectional link */
    }

    /* 5. Frame conditions: the list must be unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Invariant: the list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
