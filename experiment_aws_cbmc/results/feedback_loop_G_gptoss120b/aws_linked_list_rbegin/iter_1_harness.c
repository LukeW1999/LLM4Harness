#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rbegin_harness(void) {
    /* 1. Declare and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *r = aws_linked_list_rbegin(&list);

    /* 4. Post‑conditions */
    /* The returned pointer must be the list's tail.prev */
    assert(r == list.tail.prev);

    /* No fields of the list may change */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 5. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
