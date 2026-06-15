#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_back_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* The function is defined to return the back element, which requires a non‑empty list */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 4. Post‑conditions */
    /* The returned pointer must be the node that was previously the tail's predecessor */
    assert(result == old.tail.prev);

    /* 5. Unchanged fields – the list must be identical to its pre‑state */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
