#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_rbegin_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state for immutability checks */
    struct aws_linked_list old = list;

    /* 3. Call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* 4. Post‑conditions */

    /* The returned pointer must be the last element, or the head if the list is empty */
    assert(rval == (aws_linked_list_empty(&list) ? &list.head : list.tail.prev));

    /* If the list is non‑empty, the returned node must be part of the list */
    if (!aws_linked_list_empty(&list)) {
        assert(aws_linked_list_node_is_in_list(rval));
    }

    /* 5. Unchanged fields – the list structure must remain identical */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
