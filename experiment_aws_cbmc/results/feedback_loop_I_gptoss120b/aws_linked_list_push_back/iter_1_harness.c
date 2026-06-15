#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_back_harness() {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before the call */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 3. Allocate a node to be inserted (non‑NULL) */
    struct aws_linked_list_node node;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, &node);

    /* 5. Postconditions on success (function is void, always succeeds) */
    /* The new node becomes the last element before the tail sentinel */
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_last);
    /* If the list was not empty, the previous last node must now point to the new node */
    if (old_last != &list.head) {
        assert(old_last->next == &node);
    }

    /* 6. Unchanged fields (invariants that must remain the same) */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    /* The head sentinel's next pointer may change (empty → non‑empty), so we do not assert it here */
    /* The allocator, length counters, etc., do not exist in this struct, so no further unchanged fields */

    /* 7. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
